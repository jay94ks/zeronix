#define __NO_EXTERN_LAPIC_VARS__ 1

#include "apic.h"
#include "acpi.h"
#include "../min86/i8259.h"
#include "../min86/i8253.h"
#include "../min86/except.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/spinlock.h>
#include <zeronix/arch/x86/cpu.h>
#include <zeronix/arch/x86/layout.h>

#define IA32_APIC_BASE_MSR          0x1b
#define IA32_APIC_BASE_ENABLE_BIT   11

// --
karch_ioapic_t ioapic[MAX_IOAPIC];
karch_ioapic_irq_t ioapic_irq[MAX_IOAPIC_IRQ];
uint8_t ioapic_n;

// --
uint8_t apic_supported;
karch_lapic_info_t lapic_info;

// --
#define LAPIC_PROBE_HZ              1000
#define LAPIC_PROBE_TICK            (LAPIC_PROBE_HZ / 10)
uint32_t lapic_probe;               // --> probe to count ticks.
uint32_t lapic_probe_tccr_n;        // --> current CCR value.
uint32_t lapic_probe_tccr_e;        // --> CCR value when the probe reached to end.
uint64_t lapic_probe_tsc_n;         // --> current TSC value.
uint64_t lapic_probe_tsc_e;         // --> TSC value when the probe reached to end.
karch_spinlock_t lapic_lock;        // --> lock to ensure concurrency.

// --
void karch_apic_collect();
void karch_lapic_enable_msr();
void karch_lapic_calibrate_clocks(uint8_t cpu);
void karch_lapic_i8259_handler(const i8259_t* t);
// --

uint8_t karch_init_apic() {
    kmemset(ioapic, 0, sizeof(ioapic));
    kmemset(ioapic_irq, 0, sizeof(ioapic_irq));
    kmemset(&lapic_info, 0, sizeof(lapic_info));
    karch_spinlock_init(&lapic_lock);

    apic_supported = 0;
    ioapic_n = 0;

    if (!kcpuinfo(KCPUF_APIC_ON_CHIP)) {
        return 0; // --> not supported.
    }

    // --> collect io-apic's.
    karch_apic_collect();
    if (ioapic_n <= 0) {
        apic_supported = 0;
        return 0;
    }

    apic_supported = 1;

    // --> set initial LAPIC address.
    lapic_info.addr = LOCAL_APIC_DEF_ADDR;
    lapic_info.vaddr = LOCAL_APIC_DEF_ADDR;
    lapic_info.eoi_addr = LAPIC_EOI;
    lapic_info.bsp_cpu_id = 0;
    lapic_info.bsp_lapic_id = 0;


    return 1;
}

/**
 * post init, called from smp.c, karch_init_smp.
 */
void karch_apic_set_bsp(uint8_t cpu_id, uint8_t lapic_id) {
    lapic_info.bsp_cpu_id = cpu_id;
    lapic_info.bsp_lapic_id = lapic_id;

    // --
    karch_cpu_t* cpu = karch_get_cpu(cpu_id);
    if (cpu) {
        cpu->is_bsp = 1;
        cpu->flags |= CPUFLAG_INIT_BSP;
    }
}

uint8_t karch_apic_supported() {
    return apic_supported;
}

void karch_apic_collect() {
    karch_acpi_madt_iter_t iter;
    karch_acpi_madt_item_t* item;
    karch_acpi_madt_iterate(&iter, 0);

    while (karch_acpi_madt_next(&iter, &item)) {
        if (item->type != ACPIMADT_IOAPIC) {
            continue;
        }
        
        karch_acpi_madt_ioapic_t* now = (karch_acpi_madt_ioapic_t*) item;
        karch_ioapic_t* cur = &ioapic[ioapic_n++];

        uint32_t version = karch_ioapic_read(now->addr, IOAPIC_VERSION);

        cur->id = now->id;
        cur->addr = now->addr;
        cur->paddr = now->addr;
        cur->gsi_base = now->intr;

        cur->pins = ((version & 0xff0000) >> 16) + 1;
    }
    
    // --
    karch_acpi_madt_iterate(&iter, 0);
    
    uint8_t cpu = 0;
    while (karch_acpi_madt_next(&iter, &item)) {
        if (item->type != ACPIMADT_LAPIC) {
            continue;
        }
        
        // --> collect all CPUs here.
        karch_acpi_madt_lapic_t* now = (karch_acpi_madt_lapic_t*) item;
        uint8_t cpu_id = cpu++;

        if (!karch_set_count_cpu(cpu)) {
            break;
        }

        karch_cpu_t* cpu_ = karch_get_cpu(cpu_id);

        cpu_->flags |= CPUFLAG_INIT_LAPIC;
        cpu_->apic_id = now->apic;
    }
}

uint32_t karch_lapic_id() {
    return karch_lapic_read(LAPIC_ID) >> 24;
}

// --
#define IOAPIC_IOREGSEL	0x0
#define IOAPIC_IOWIN	0x10
// --

uint32_t karch_ioapic_read(uint32_t iobase, uint32_t reg) {
    *((volatile uint32_t*)(iobase + IOAPIC_IOREGSEL)) = reg;
    return *((volatile uint32_t*)(iobase + IOAPIC_IOWIN));
}

void karch_ioapic_write(uint32_t iobase, uint32_t reg, uint32_t val) {
    *((volatile uint32_t*)(iobase + IOAPIC_IOREGSEL)) = reg;
    *((volatile uint32_t*)(iobase + IOAPIC_IOWIN)) = val;
}

uint32_t karch_ioapic_enable_pin(uint32_t ioapic_addr, uint32_t pin) {
    uint32_t lo = karch_ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);
	karch_ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo & ~APIC_ICR_INT_MASK);
}

uint32_t karch_ioapic_disable_pin(uint32_t ioapic_addr, uint32_t pin) {
    uint32_t lo = karch_ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);
	karch_ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo | APIC_ICR_INT_MASK);
}

uint32_t karch_lapic_read(uint32_t iobase) {
    return *((volatile uint32_t*)(iobase));
}

void karch_lapic_write(uint32_t iobase, uint32_t val) {
    do { 
        *((volatile uint32_t*)(iobase)) = val;
    }
    while(0);
}

uint32_t karch_lapic_error() {
    karch_lapic_write(LAPIC_ESR, 0);
    return karch_lapic_read(LAPIC_ESR);
}

uint8_t karch_lapic_eoi() {
    if (lapic_info.eoi_addr == 0) {
        return 0;
    }

    do {
        *((volatile uint32_t*) lapic_info.eoi_addr) = 0;
    }
    while(0);
    return 1;
}

/**
 * set APIC enable bit on MSR.
 */
void karch_lapic_enable_msr() {
    uint32_t hi, lo;

    read_msr(IA32_APIC_BASE_MSR, &hi, &lo);

    lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
    write_msr(IA32_APIC_BASE_MSR, hi, lo);

    return 1;
}

uint8_t karch_lapic_enable(uint8_t cpu) {
    if (!lapic_info.addr) {
        return 0;
    }

    // --> TSC is required to enable LAPIC.
    if (!kcpuinfo(KCPUF_TSC)) {
        return 0;
    }

    // --> if no CPU entry available.
    if (!karch_get_cpu(cpu)) {
        return 0;
    }

    karch_lapic_enable_msr();

    // --> set priority.
    karch_lapic_write(LAPIC_TPR, 0);
    uint32_t val = karch_lapic_error();

    // --> enable lapic and set spurious vector to 0xff.
    val = karch_lapic_read(LAPIC_SIVR);
    val |= APIC_ENABLE | APIC_SPURIOUS_INT_VECTOR;
    val &= ~APIC_FOCUS_DISABLED;
    karch_lapic_write(LAPIC_SIVR, val);
    karch_lapic_read(LAPIC_SIVR);

    // --> clear EOI initially.
    karch_lapic_eoi();

    // --> set LDR (logical destination register).
    val = karch_lapic_read(LAPIC_LDR) & ~0xff000000;
    val |= (cpu & 0xff) << 24;
    karch_lapic_write(LAPIC_LDR, val);

    // --> set DFR (destination format register) to flat mode.
    val = karch_lapic_read(LAPIC_DFR) | 0xf0000000;
    karch_lapic_write(LAPIC_DFR, val);

    val = karch_lapic_read(LAPIC_LVTER) & 0xffffff00;
    karch_lapic_write(LAPIC_LVTER, val);

    // --
    uint8_t nlvt = (karch_lapic_read(LAPIC_VERSION) >> 16) & 0xff;
    if (nlvt >= 4) {
        val = karch_lapic_read(LAPIC_LVTTMR);
        karch_lapic_write(LAPIC_LVTTMR, val | APIC_ICR_INT_MASK);
    }

    if (nlvt >= 5) {
        val = karch_lapic_read(LAPIC_LVTPCR);
        karch_lapic_write(LAPIC_LVTPCR, val | APIC_ICR_INT_MASK);
    }

    // --> setup TPR to allow all interrupts.
    val = karch_lapic_read(LAPIC_TPR);

    // --> accept all interrupts.
    karch_lapic_write(LAPIC_TPR, val & ~0xff);

    karch_lapic_read(LAPIC_SIVR);
    karch_lapic_eoi();
    
    karch_lapic_calibrate_clocks(cpu);
    return 1;
}

void karch_lapic_read_tsc64(uint64_t* val) {
    uint32_t lo, hi;

    cpu_read_tsc(&hi, &lo);
    if (val) {
       *val = (((uint64_t)hi) << 32) | lo;
    }
}

void karch_lapic_calibrate_clocks(uint8_t cpu) {
    uint32_t val = 0xffffffffu, lvtt;
    karch_cpu_t* cpu_ = karch_get_cpu(cpu);

    if (!cpu_) {
        // --> no CPU entry found.
        return;
    }

    // --> this function must be called one by one.
    karch_spinlock_lock(&lapic_lock);

    // --> initial count register to high.
    //   : so it'll not underflow during testing.
    karch_lapic_write(LAPIC_TIMER_ICR, val);
    
    // --> set current count register.
    val = 0;
    karch_lapic_write(LAPIC_TIMER_CCR, val);

    // --
    lvtt = karch_lapic_read(LAPIC_TIMER_DCR) & ~0x0b;
    
    // --> set divide configuration register to 1.
    lvtt = APIC_TDCR_1;
    karch_lapic_write(LAPIC_TIMER_DCR, lvtt);

    // --> from minix /arch/i386/apic.c, 480.
    lvtt = karch_lapic_read(LAPIC_LVTTR);
    lvtt |= APIC_LVTT_MASK;
    karch_lapic_write(LAPIC_LVTTR, lvtt);

    // -----------
    karch_i8259_cb_t prev_tmr, prev_spu;
    void* prev_tmr_data, *prev_spu_data;

    // --> backup previous interrupt handling callback here.
    karch_get_hwint_i8259(I8259_TIMER, &prev_tmr, &prev_tmr_data);
    karch_get_hwint_i8259(I8259_LPT1, &prev_spu, &prev_spu_data);

    // --> and set the tester interrupt handler callback.
    karch_set_hwint_i8259(I8259_TIMER, karch_lapic_i8259_handler, 0);
    karch_set_hwint_i8259(I8259_LPT1, karch_lapic_i8259_handler, 0);

    // --> enable i8253 timer here.
    karch_init_i8253(LAPIC_PROBE_HZ);
    karch_unmask_i8259(I8259_TIMER);

    // --> reset probe.
    lapic_probe = 0;

    // --> enable interrupt.
    cpu_sti();

    while (lapic_probe < LAPIC_PROBE_TICK) {
        cpu_sti();
    }

    // --> disable interrupt.
    cpu_cli();

    // --> restore previous interrupt handlers.
    karch_set_hwint_i8259(I8259_TIMER, prev_tmr, prev_tmr_data);
    karch_set_hwint_i8259(I8259_LPT1, prev_spu, prev_spu_data);

    // --> then, disable i8253 timer.
    karch_mask_i8259(I8259_TIMER);
    karch_deinit_i8253();

    uint32_t ccr_delta = lapic_probe_tccr_n - lapic_probe_tccr_e;
    uint64_t tsc_delta = lapic_probe_tsc_e - lapic_probe_tsc_n;
    
    // --> unlock.
    karch_spinlock_unlock(&lapic_lock);

    // --> set frequency informations here.
    uint64_t cpu_freq = (tsc_delta / (LAPIC_PROBE_TICK - 1)) * LAPIC_PROBE_HZ;
    cpu_->lapic_freq = (LAPIC_PROBE_HZ * ccr_delta) / (LAPIC_PROBE_TICK - 1);
    cpu_->freq = (uint32_t)(cpu_freq / 1000000);
    
    // --> set frequency flags here.
    cpu_->flags |= CPUFLAG_INIT_FREQ | CPUFLAG_INIT_LAPIC_FREQ;
}

void karch_lapic_i8259_handler(const i8259_t* t) {
    if (t->n == I8259_TIMER) {
        uint32_t tcrt;
        uint64_t tsc;
        
        lapic_probe++;
        
        karch_lapic_read_tsc64(&tsc);
        tcrt = karch_lapic_read(LAPIC_TIMER_CCR);

        if (lapic_probe < LAPIC_PROBE_TICK) {
            lapic_probe_tccr_n = tcrt;
            lapic_probe_tsc_n = tsc;
        }

        else {
            lapic_probe_tccr_e = tcrt;
            lapic_probe_tsc_e = tsc;
            karch_deinit_i8253();
        }
    }
    
    else if (t->n == I8259_LPT1) {
        // --> spurious: do nothing.
    }
}

uint8_t karch_lapic_oneshot_timer(uint32_t usec) {
    const karch_cpu_t* cpu = karch_get_cpu_current();
    if (!cpu) {
        return 0;
    }

    uint32_t tpus = cpu->lapic_freq / 1000000;
    karch_lapic_write(LAPIC_TIMER_ICR, usec * tpus);
    karch_lapic_write(LAPIC_TIMER_DCR, APIC_TDCR_1);
    karch_lapic_write(LAPIC_LVTTR, APIC_TIMER_INT_VECTOR);
    return 1;
}

uint8_t karch_lapic_periodic_timer(uint32_t freq) {
    const karch_cpu_t* cpu = karch_get_cpu_current();
    if (!cpu) {
        return 0;
    }

    karch_lapic_write(LAPIC_TIMER_DCR, APIC_TDCR_1);
    karch_lapic_write(LAPIC_LVTTR, APIC_LVTT_TM | APIC_TIMER_INT_VECTOR);
    karch_lapic_write(LAPIC_TIMER_ICR, cpu->lapic_freq / freq);
    return 1;
}

void karch_lapic_stop_timer() {
    uint32_t lvttr = karch_lapic_read(LAPIC_LVTTR);
	karch_lapic_write(LAPIC_LVTTR, lvttr | APIC_LVTT_MASK);
    karch_lapic_write(LAPIC_TIMER_ICR, 0);
    karch_lapic_write(LAPIC_TIMER_DCR, 0);
}

void karch_lapic_udelay(uint32_t n) {
	karch_lapic_oneshot_timer(n);
	while (karch_lapic_read(LAPIC_TIMER_CCR)) {
		cpu_pause();
    }
}

void karch_lapic_mdelay(uint32_t ms) {
    while (ms > 0) {
        uint32_t slice = ms > 16 ? 16 : ms;

        // --> delay in 16k usec unit.
        ms -= slice;
        karch_lapic_udelay(slice * 1000);
    }
}

karch_lapic_wr_t karch_lapic_wait_flag_set(uint32_t addr, uint32_t flag, uint32_t tries) {
    uint32_t err;

    while (1) {
        if ((karch_lapic_read(addr) & flag) == flag) {
            break;
        }

        if ((err = karch_lapic_error()) != 0) {
            return LAPICWR_ERROR;
        }
    
        if (!tries) {
            return LAPICWR_EXHAUSTED;
        }

        tries--;
    }

    return LAPICWR_SUCCESS;
}

karch_lapic_wr_t karch_lapic_wait_flag_clear(uint32_t addr, uint32_t flag, uint32_t tries) {
    uint32_t err;

    while (1) {
        if ((karch_lapic_read(addr) & flag) != flag) {
            break;
        }

        if ((err = karch_lapic_error()) != 0) {
            return LAPICWR_ERROR;
        }
    
        if (!tries) {
            return LAPICWR_EXHAUSTED;
        }

        tries--;
    }

    return LAPICWR_SUCCESS;
}
