#define __ARCH_X86_INTERNALS__ 1
#define __NO_EXTERN_LAPIC_VARS__ 1
#include <x86/peripherals/apic.h>
#include <x86/peripherals/acpi.h>

#include <x86/env.h>
#include <x86/spinlock.h>
#include <x86/klib.h>

#include <x86/k86/irq.h>
#include <x86/k86/tables.h>
#include <x86/legacy/i8253.h>
#include <x86/legacy/i8259.h>
#include <x86/smp.h>

#include <zeronix/kstring.h>

// --
#define IA32_APIC_BASE_MSR          0x1b
#define IA32_APIC_BASE_ENABLE_BIT   11

#define LAPIC_PROBE_HZ              1000
#define LAPIC_PROBE_TICK            (LAPIC_PROBE_HZ / 10)

#define IOAPIC_IOREGSEL	0x0
#define IOAPIC_IOWIN	0x10

#define RTC_INDEX   0x70
#define RTC_IO      0x71

#if 1
void karch_apic_hwint_00();
void karch_apic_hwint_01();
void karch_apic_hwint_02();
void karch_apic_hwint_03();
void karch_apic_hwint_04();
void karch_apic_hwint_05();
void karch_apic_hwint_06();
void karch_apic_hwint_07();
void karch_apic_hwint_08();
void karch_apic_hwint_09();

void karch_apic_hwint_10();
void karch_apic_hwint_11();
void karch_apic_hwint_12();
void karch_apic_hwint_13();
void karch_apic_hwint_14();
void karch_apic_hwint_15();
void karch_apic_hwint_16();
void karch_apic_hwint_17();
void karch_apic_hwint_18();
void karch_apic_hwint_19();

void karch_apic_hwint_20();
void karch_apic_hwint_21();
void karch_apic_hwint_22();
void karch_apic_hwint_23();
void karch_apic_hwint_24();
void karch_apic_hwint_25();
void karch_apic_hwint_26();
void karch_apic_hwint_27();
void karch_apic_hwint_28();
void karch_apic_hwint_29();

void karch_apic_hwint_30();
void karch_apic_hwint_31();
void karch_apic_hwint_32();
void karch_apic_hwint_33();
void karch_apic_hwint_34();
void karch_apic_hwint_35();
void karch_apic_hwint_36();
void karch_apic_hwint_37();
void karch_apic_hwint_38();
void karch_apic_hwint_39();

void karch_apic_hwint_40();
void karch_apic_hwint_41();
void karch_apic_hwint_42();
void karch_apic_hwint_43();
void karch_apic_hwint_44();
void karch_apic_hwint_45();
void karch_apic_hwint_46();
void karch_apic_hwint_47();
void karch_apic_hwint_48();
void karch_apic_hwint_49();

void karch_apic_hwint_50();
void karch_apic_hwint_51();
void karch_apic_hwint_52();
void karch_apic_hwint_53();
void karch_apic_hwint_54();
void karch_apic_hwint_55();
void karch_apic_hwint_56();
void karch_apic_hwint_57();
void karch_apic_hwint_58();
void karch_apic_hwint_59();

void karch_apic_hwint_60();
void karch_apic_hwint_61();
void karch_apic_hwint_62();
void karch_apic_hwint_63();

void karch_apic_zbint_f0();
void karch_apic_zbint_f1();
void karch_apic_zbint_f2();
void karch_apic_zbint_fe();
void karch_apic_zbint_ff();
#endif

#define APIC_HW_IRQ_VECTOR_OFFSET       0x50
static const struct gatevec gv_apic_hw[] = {
    { karch_apic_hwint_00,  0, PRIV_KERN },
    { karch_apic_hwint_01,  1, PRIV_KERN },
    { karch_apic_hwint_02,  2, PRIV_KERN },
    { karch_apic_hwint_03,  3, PRIV_KERN },
    { karch_apic_hwint_04,  4, PRIV_KERN },
    { karch_apic_hwint_05,  5, PRIV_KERN },
    { karch_apic_hwint_06,  6, PRIV_KERN },
    { karch_apic_hwint_07,  7, PRIV_KERN },
    { karch_apic_hwint_08,  8, PRIV_KERN },
    { karch_apic_hwint_09,  9, PRIV_KERN },
    
    { karch_apic_hwint_10, 10, PRIV_KERN },
    { karch_apic_hwint_11, 11, PRIV_KERN },
    { karch_apic_hwint_12, 12, PRIV_KERN },
    { karch_apic_hwint_13, 13, PRIV_KERN },
    { karch_apic_hwint_14, 14, PRIV_KERN },
    { karch_apic_hwint_15, 15, PRIV_KERN },
    { karch_apic_hwint_16, 16, PRIV_KERN },
    { karch_apic_hwint_17, 17, PRIV_KERN },
    { karch_apic_hwint_18, 18, PRIV_KERN },
    { karch_apic_hwint_19, 19, PRIV_KERN },
    
    { karch_apic_hwint_20, 20, PRIV_KERN },
    { karch_apic_hwint_21, 21, PRIV_KERN },
    { karch_apic_hwint_22, 22, PRIV_KERN },
    { karch_apic_hwint_23, 23, PRIV_KERN },
    { karch_apic_hwint_24, 24, PRIV_KERN },
    { karch_apic_hwint_25, 25, PRIV_KERN },
    { karch_apic_hwint_26, 26, PRIV_KERN },
    { karch_apic_hwint_27, 27, PRIV_KERN },
    { karch_apic_hwint_28, 28, PRIV_KERN },
    { karch_apic_hwint_29, 29, PRIV_KERN },
    
    { karch_apic_hwint_30, 30, PRIV_KERN },
    { karch_apic_hwint_31, 31, PRIV_KERN },
    { karch_apic_hwint_32, 32, PRIV_KERN },
    { karch_apic_hwint_33, 33, PRIV_KERN },
    { karch_apic_hwint_34, 34, PRIV_KERN },
    { karch_apic_hwint_35, 35, PRIV_KERN },
    { karch_apic_hwint_36, 36, PRIV_KERN },
    { karch_apic_hwint_37, 37, PRIV_KERN },
    { karch_apic_hwint_38, 38, PRIV_KERN },
    { karch_apic_hwint_39, 39, PRIV_KERN },
        
    { karch_apic_hwint_40, 40, PRIV_KERN },
    { karch_apic_hwint_41, 41, PRIV_KERN },
    { karch_apic_hwint_42, 42, PRIV_KERN },
    { karch_apic_hwint_43, 43, PRIV_KERN },
    { karch_apic_hwint_44, 44, PRIV_KERN },
    { karch_apic_hwint_45, 45, PRIV_KERN },
    { karch_apic_hwint_46, 46, PRIV_KERN },
    { karch_apic_hwint_47, 47, PRIV_KERN },
    { karch_apic_hwint_48, 48, PRIV_KERN },
    { karch_apic_hwint_49, 49, PRIV_KERN },
    
    { karch_apic_hwint_50, 50, PRIV_KERN },
    { karch_apic_hwint_51, 51, PRIV_KERN },
    { karch_apic_hwint_52, 52, PRIV_KERN },
    { karch_apic_hwint_53, 53, PRIV_KERN },
    { karch_apic_hwint_54, 54, PRIV_KERN },
    { karch_apic_hwint_55, 55, PRIV_KERN },
    { karch_apic_hwint_56, 56, PRIV_KERN },
    { karch_apic_hwint_57, 57, PRIV_KERN },
    { karch_apic_hwint_58, 58, PRIV_KERN },
    { karch_apic_hwint_59, 59, PRIV_KERN },
    
    { karch_apic_hwint_60, 60, PRIV_KERN },
    { karch_apic_hwint_61, 61, PRIV_KERN },
    { karch_apic_hwint_62, 62, PRIV_KERN },
    { karch_apic_hwint_63, 63, PRIV_KERN },

    GATEVEC_NULL
};

static const struct gatevec gv_apic_zb[] = {
    { karch_apic_zbint_f1, LAPICIRQ_SCHEDULE, PRIV_KERN }, // --> sched proc.  (SMP only)
    { karch_apic_zbint_f2, LAPICIRQ_CPU_HALT, PRIV_KERN }, // --> CPU halt.    (SMP only)
    { karch_apic_zbint_fe, LAPICIRQ_SPURIOUS, PRIV_KERN }, // --> spurious int vec.
    { karch_apic_zbint_ff, LAPICIRQ_ERROR, PRIV_KERN }, // --> error int vec.
    GATEVEC_NULL
};

static const struct gatevec gv_apic_bsp[] = {
    { karch_apic_zbint_f0, LAPICIRQ_TIMER, PRIV_KERN }, // --> timer.  (SMP only)
    GATEVEC_NULL
};

// --
#define MAKE_IRQ_OVR(irq, pol, trg)     ((irq) | ((pol) << 8) | ((trg) << 10))
#define GET_IRQ_OVR(ovr)                ((ovr) & 0xff)
#define GET_IRQ_POL(ovr)                (((ovr) >> 8) && 0x03)
#define GET_IRQ_TRG(ovr)                (((ovr) >> 10) && 0x03)

// --
uint8_t apic_supported;
karch_lapic_info_t lapic_info;

// --
karch_ioapic_t ioapic[MAX_IOAPIC];
karch_ioapic_irq_t ioapic_irq[MAX_IOAPIC_IRQ];
uint16_t ioapic_irq_ovr[MAX_IOAPIC_IRQ];    // --> stores POL (2), TRG (2), SRC_INTR (8).
uint8_t ioapic_irq_rvo[MAX_IOAPIC_IRQ];     // --> stores GBL_INTR.
uint8_t ioapic_n;

// --
karch_lapic_t lapic[MAX_CPU];
uint8_t lapic_map[0xff + 1];        // --> for fast mapping: lapic_map[n] == LAPIC_ID.
uint8_t lapic_n;
uint8_t lapic_bsp;

// --
uint32_t lapic_probe;               // --> probe to count ticks.
uint32_t lapic_probe_tccr_n;        // --> current CCR value.
uint32_t lapic_probe_tccr_e;        // --> CCR value when the probe reached to end.
uint64_t lapic_probe_tsc_n;         // --> current TSC value.
uint64_t lapic_probe_tsc_e;         // --> TSC value when the probe reached to end.
karch_spinlock_t lapic_lock;        // --> lock to ensure concurrency.

// -- 
uint8_t karch_apic_supported() {
    if (!karch_acpi_supported()) {
        return 0;
    }

    return apic_supported;
}

// --
void karch_apic_collect();
uint8_t karch_apic_exists();

void karch_lapic_calibrate_clocks(uint8_t n);
void karch_lapic_irq_handler(karch_irq_t* t);
// --

uint8_t karch_apic_init() {
    kmemset(ioapic, 0, sizeof(ioapic));
    kmemset(ioapic_irq, 0, sizeof(ioapic_irq));

    kmemset(lapic, 0, sizeof(lapic));
    kmemset(&lapic_info, 0, sizeof(lapic_info));
    karch_spinlock_init(&lapic_lock);

    apic_supported = 0;
    ioapic_n = lapic_n = 0;

    // --> set IRQ as identity mapping.
    for (uint8_t i = 0; i < MAX_IOAPIC_IRQ; ++i) {
        ioapic_irq_ovr[i] = i;
        ioapic_irq_rvo[i] = i;
    }

    if (!karch_apic_exists()) {
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
    
    // --> set the current LAPIC as BSP.
    karch_lapic_t* lapic = karch_lapic_get_current();

    lapic_bsp = 0;
    if (lapic) {
        lapic->bsp = 1;
        lapic_bsp = lapic->id;
    }

    // --> we need to get replacement mapping for IRQ0 (PIT) and IRQ8.
    //   : https://wiki.osdev.org/HPET#.22Legacy_replacement.22_mapping
    
    return 1;
}


/**
 * test whether the CPU has APIC or not.
*/
uint8_t karch_apic_exists() {
    karch_cpuinfo_t info;

    if (!karch_env_cpuinfo(&info)) {
        return 0;
    }

    uint32_t edx = info.feature[CPUINFO_EDX];
    return (edx & CPUID1_EDX_APIC_ON_CHIP) != 0 ? 1 : 0;
}

/**
 * collect all IO-APICs.
 */
void karch_apic_collect() {
    karch_acpi_madt_iter_t iter;
    karch_acpi_madt_item_t* item;
    karch_acpi_madt_iterate(&iter, 0);

    while (karch_acpi_madt_next(&iter, &item)) {
        if (item->type == ACPIMADT_IOAPIC) {
            karch_acpi_madt_ioapic_t* now = (karch_acpi_madt_ioapic_t*) item;
            karch_ioapic_t* cur = &ioapic[ioapic_n++];

            uint32_t version = karch_ioapic_read(now->addr, IOAPIC_VERSION);

            cur->id = now->id;
            cur->addr = now->addr;
            cur->paddr = now->addr;
            cur->gsi_base = now->intr;

            cur->pins = ((version & 0xff0000) >> 16) + 1;
        }
        
        else if (item->type == ACPIMADT_INT_SRC) {
            karch_acpi_madt_intsrc_t* now = (karch_acpi_madt_intsrc_t*) item;

            // --> take only EISA / ISA.
            if (now->bus != 0) {
                continue;
            }

            uint8_t pol = ACPI_INTSRC_GET_POLARITY(now->flags);
            uint8_t trg = ACPI_INTSRC_GET_TRIGGER_MODE(now->flags);

            // --> store mapping info here.
            ioapic_irq_ovr[now->global_intr] 
                = MAKE_IRQ_OVR(now->source, pol, trg);

            ioapic_irq_rvo[now->source] = now->global_intr;
        }
    }
    
    // --
    karch_acpi_madt_iterate(&iter, 0);
    lapic_n = 0;

    while (karch_acpi_madt_next(&iter, &item)) {
        if (item->type != ACPIMADT_LAPIC) {
            continue;
        }
        
        karch_acpi_madt_lapic_t* now = (karch_acpi_madt_lapic_t*) item;
        uint8_t n = lapic_n++;

        if (n >= MAX_CPU) {
            break;
        }

        lapic[n].id = now->apic;
        lapic_map[now->apic] = n;
    }
}

int32_t karch_lapic_id() {
    if (!karch_apic_supported()) {
        return -1;
    }

    return karch_lapic_read(LAPIC_ID) >> 24;
}

int32_t karch_lapic_number() {
    int32_t lapic_id = karch_lapic_id();
    if (lapic_id < 0) {
        return -1;
    }

    uint8_t real_id = lapic_id & 0xff;
    uint8_t n = lapic_map[real_id];
    if (n >= lapic_n) {
        return -1;
    }
    
    return n;
}

uint32_t karch_lapic_count() {
    return lapic_n;
}

karch_lapic_t* karch_lapic_get(uint8_t n) {
    if (n >= lapic_n) {
        return 0;
    }

    return &lapic[n];
}

karch_lapic_t* karch_lapic_get_current() {
    int32_t n = karch_lapic_number();
    if (n < 0) {
        return 0;
    }
    
    return &lapic[n];
}

uint32_t karch_ioapic_read(uint32_t iobase, uint32_t reg) {
    *((volatile uint32_t*)(iobase + IOAPIC_IOREGSEL)) = reg;
    return *((volatile uint32_t*)(iobase + IOAPIC_IOWIN));
}

void karch_ioapic_write(uint32_t iobase, uint32_t reg, uint32_t val) {
    *((volatile uint32_t*)(iobase + IOAPIC_IOREGSEL)) = reg;
    *((volatile uint32_t*)(iobase + IOAPIC_IOWIN)) = val;
}

void karch_ioapic_enable_pin(uint32_t ioapic_addr, uint32_t pin) {
    uint32_t lo = karch_ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);
	karch_ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo & ~APIC_ICR_INT_MASK);
}

void karch_ioapic_disable_pin(uint32_t ioapic_addr, uint32_t pin) {
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
 * Called from apic_idt.asm.
 * `n` value: refer `gv_apic_hw`.
*/
void karch_apic_hwint(uint32_t n, uint32_t k, karch_intr_frame_t* frame) {
    karch_irq_intr_begin(frame);

    if (n <= MAX_IOAPIC_IRQ && ioapic_irq[n].apic) {
        uint8_t ovr = GET_IRQ_OVR(ioapic_irq_ovr[n]);

        karch_ioapic_disable_irq(n);
        // --> translate to kernel's IRQn.
        karch_irq_dispatch(ovr);
        karch_ioapic_enable_irq(n);

        // --> end of interrupt.
        if (ioapic_irq[n].eoi) {
            ioapic_irq[n].eoi(n);
        }
    }

    else {
        karch_lapic_eoi();
    }

    if (!k) {
        // --> switch to user if possible.
    }

    karch_irq_intr_end();
}

/**
 * Called from apic_idt.asm.
 * `n` value: refer `gv_apic_zb`.
*/
void karch_apic_zbint(uint32_t n, uint32_t k, karch_intr_frame_t* frame) {
    karch_irq_intr_begin(frame);

    if (n >= 0xf0 && n <= 0xff) {
        karch_irq_mask(n);
        karch_irq_dispatch(n);
        karch_irq_unmask(n);
    }

    // --> emit EIO to Local APIC.
    karch_lapic_eoi();
    karch_irq_intr_end();
    
    if (!k) {
        // --> switch to user if possible.
    }
}

void karch_lapic_enable_msr() {
    uint32_t hi, lo;

    read_msr(IA32_APIC_BASE_MSR, &hi, &lo);

    lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
    write_msr(IA32_APIC_BASE_MSR, hi, lo);
}

uint8_t karch_lapic_check_tsc() {
    karch_cpuinfo_t info;

    if (!karch_env_cpuinfo(&info)) {
        return 0;
    }

    uint32_t edx = info.feature[CPUINFO_EDX];
    return (edx & CPUID1_EDX_TSC) != 0 ? 1 : 0;
}

uint8_t karch_lapic_enable(uint8_t n) {
    if (!lapic_info.addr) {
        return 0;
    }

    // --> TSC is required to enable LAPIC.
    if (!karch_lapic_check_tsc()) {
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
    val |= (n & 0xff) << 24;
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
    
    karch_lapic_calibrate_clocks(n);
    return 1;
}

void karch_lapic_read_tsc64(uint64_t* val) {
    uint32_t lo, hi;

    cpu_read_tsc(&hi, &lo);
    if (val) {
       *val = (((uint64_t)hi) << 32) | lo;
    }
}

void karch_lapic_calibrate_clocks(uint8_t n) {
    uint32_t val = 0xffffffffu, lvtt;
    
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
    karch_irq_t irq_timer, irq_spu;

    irq_timer.handler = karch_lapic_irq_handler;
    irq_spu.handler = karch_lapic_irq_handler;
    
    // --> reset probe.
    lapic_probe = 0;

    // --> hook timer and spurious IRQs.
    karch_irq_register(IRQN_TIMER, &irq_timer);
    karch_irq_register(IRQN_LPT1, &irq_spu);  // --> spurious IRQ.

    // --> enable i8253 timer here.
    karch_i8253_init(LAPIC_PROBE_HZ);

    while (lapic_probe < LAPIC_PROBE_TICK) {
        cpu_sti();
        cpu_mfence();
    }

    // --> then, disable i8253 timer.
    karch_i8253_deinit();

    // --> unhook IRQs.
    karch_irq_unregister(&irq_timer);
    karch_irq_unregister(&irq_spu);
    
    uint32_t ccr_delta = lapic_probe_tccr_n - lapic_probe_tccr_e;
    uint64_t tsc_delta = lapic_probe_tsc_e - lapic_probe_tsc_n;
    
    // --> unlock.
    karch_spinlock_unlock(&lapic_lock);

    // --> set frequency informations here.
    uint64_t cpu_freq = (tsc_delta / (LAPIC_PROBE_TICK - 1)) * LAPIC_PROBE_HZ;
    lapic[n].lapic_freq = (LAPIC_PROBE_HZ * ccr_delta) / (LAPIC_PROBE_TICK - 1);
    lapic[n].freq = (uint32_t)(cpu_freq / 1000000);
    lapic[n].ready = 1;
}

void karch_lapic_irq_handler(karch_irq_t* t) {
    if (t->n == IRQN_TIMER) {
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
            karch_i8253_deinit();
        }
    }
    /*
    // --> ignore others.
    else if (t->n == IRQN_LPT1) {
        // --> spurious: do nothing.
    }
    */
}

uint8_t karch_lapic_oneshot_timer(uint32_t usec) {
    karch_lapic_t* lapic = karch_lapic_get_current();
    if (!lapic) {
        return 0;
    }

    uint32_t tpus = lapic->lapic_freq / 1000000;
    karch_lapic_write(LAPIC_TIMER_DCR, APIC_TDCR_1);
    karch_lapic_write(LAPIC_LVTTR, APIC_TIMER_INT_VECTOR);
    karch_lapic_write(LAPIC_TIMER_ICR, usec * tpus);
    return 1;
}

uint8_t karch_lapic_periodic_timer(uint32_t freq) {
    karch_lapic_t* lapic = karch_lapic_get_current();
    if (!lapic) {
        return 0;
    }

    karch_lapic_write(LAPIC_TIMER_DCR, APIC_TDCR_1);
    karch_lapic_write(LAPIC_LVTTR, APIC_LVTT_TM | APIC_TIMER_INT_VECTOR);
    karch_lapic_write(LAPIC_TIMER_ICR, lapic->lapic_freq / freq);
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

void karch_apic_load_idt() {
    karch_lapic_t* lapic = karch_lapic_get_current();
    
    // --> load hardware interrupt, + 0x50.
    karch_tables_load_idt(gv_apic_hw, APIC_HW_IRQ_VECTOR_OFFSET);

    // --> load special interrupts, as-is.
    karch_tables_load_idt(gv_apic_zb, 0x00);

    // --> error interrupt vector.
    uint32_t val = karch_lapic_read(LAPIC_LVTER);
    val |= APIC_ERROR_INT_VECTOR;
    val &= ~ APIC_ICR_INT_MASK;
    karch_lapic_write(LAPIC_LVTER, val);
	(void)  karch_lapic_read(LAPIC_LVTER);

    // --> load timer interrupt vector.
    if (lapic->bsp) {
        karch_tables_load_idt(gv_apic_bsp, 0x00);
    }
}

void karch_apic_reset_idt() {
    // --> reset IDT.
    karch_tables_reset_idt();

    // --> then, load min86's PIC interrupt vectors.
    karch_i8259_setup_idt();

    // --> flush IDT table.
    karch_tables_flush_idt();

    // --> then, re-enable ICMR of i8259.
    karch_i8259_imcr_enable();
}

void karch_lapic_set_ipi(uint32_t apic_id, uint32_t mode) {
    // --> write ICR2: set the target PE.
    uint32_t icr2 = karch_lapic_read(LAPIC_ICR2);
    karch_lapic_write(LAPIC_ICR2, (icr2 & 0xffffff) | ((apic_id & 0xff) << 24));

    // --> write ICR1.
    uint32_t icr1 = karch_lapic_read(LAPIC_ICR1);
    karch_lapic_write(LAPIC_ICR1, (icr1 & 0xfff32000) | mode);
    
    // --> delay 200 usec.
	karch_lapic_udelay(200);
}

uint8_t karch_lapic_send_init_ipi(uint8_t n) {
    karch_lapic_t* lapic = karch_lapic_get(n);
    if (!lapic) {
        return 0;
    }

    karch_lapic_wr_t wr;
    uint8_t apic_id = lapic->id;
    
    // --> set bios shutdown code to 0x0a.
    cpu_out8(RTC_INDEX, 0x0f);
    cpu_out8(RTC_IO, 0x0a);

    // --> clear error state register.
    karch_lapic_error();

    // --> init ipl, no shorthand, dest mode: phy.
    karch_lapic_set_ipi(apic_id, APIC_ICR_DM_INIT | APIC_ICR_LEVEL_ASSERT);
    
    // --> wait for pending flag to be cleared.
    wr = karch_lapic_wait_flag_clear(LAPIC_ICR1, APIC_ICR_DELIVERY_PENDING, 1000);
    if (wr != LAPICWR_SUCCESS) {
        return 0; // --> skip current CPU.
    }

    // --> clear error state register again.
    karch_lapic_error();

    // --> de-init ipl, no shorthand, dest mode: phy.
    karch_lapic_set_ipi(apic_id, APIC_ICR_DEST_ALL);

    // --> wait for pending flag to be cleared.
    wr = karch_lapic_wait_flag_clear(LAPIC_ICR1, APIC_ICR_DELIVERY_PENDING, 1000);
    if (wr != LAPICWR_SUCCESS) {
        return 0; // --> skip current CPU.
    }
    
    // --> clear error state register again.
    karch_lapic_error();

    // --> sleep 10 millisec.
	karch_lapic_mdelay(10);
    return 1;
}

uint8_t karch_lapic_send_startup_ipi(uint8_t n, uint32_t entry_point) {
    karch_lapic_t* lapic = karch_lapic_get(n);
    if (!lapic) {
        return 0;
    }

    const uint32_t refined_ep = (entry_point >> 12) & 0xff;
    karch_lapic_wr_t wr;

    // --> send SIPI-SIPI sequence.
    for (uint8_t i = 0; i < 2; ++i) {
        // --> clear the error status.
        karch_lapic_error();

        // --> send SIPI.
        karch_lapic_set_ipi(lapic->id,
            APIC_ICR_LEVEL_ASSERT | APIC_ICR_DM_STARTUP |
            refined_ep);

        // --> wait for pending flag to be cleared.
        wr = karch_lapic_wait_flag_clear(LAPIC_ICR1, APIC_ICR_DELIVERY_PENDING, 1000);
        if (wr != LAPICWR_SUCCESS) {
            return 0; // --> skip current CPU.
        }
    }

	return 1;
}

uint8_t karch_lapic_pending_ipi() {
    if (!apic_supported) {
        return 0;
    }

    return (karch_lapic_read(LAPIC_ICR1) & APIC_ICR_DELIVERY_PENDING) != 0 ? 1 : 0;
}

uint8_t karch_lapic_emit_ipi(uint8_t vector, uint32_t n, karch_lapic_ipi_t how) {
    if (lapic_n <= 1) {
        return 0;
    }

    while(1) {
        if (karch_lapic_pending_ipi()) {
            cpu_pause();
            continue;
        }

        break;
    }

    karch_lapic_t* lapic = karch_lapic_get(n);
    uint32_t icr1 = karch_lapic_read(LAPIC_ICR1) & 0xfff0f800;
    uint32_t icr2 = karch_lapic_read(LAPIC_ICR2) & 0xffffff;

    switch (how) {
        case LAPICIPI_SPECIFIC:
            if (!lapic->ready) {
                return 0;
            }

            karch_lapic_write(LAPIC_ICR2, icr2 | (((uint32_t)lapic->id) << 24));
            karch_lapic_write(LAPIC_ICR1, icr1 | APIC_ICR_DEST_FIELD | vector);
            break;
            
        case LAPICIPI_SELF:
            karch_lapic_write(LAPIC_ICR2, icr2);
            karch_lapic_write(LAPIC_ICR1, icr1 | APIC_ICR_DEST_SELF | vector);
            break;
            
        case LAPICIPI_BROADCAST:
            karch_lapic_write(LAPIC_ICR2, icr2);
            karch_lapic_write(LAPIC_ICR1, icr1 | APIC_ICR_DEST_ALL_BUT_SELF | vector);
            break;
            
        case LAPICIPI_EXCEPT_SELF:
            karch_lapic_write(LAPIC_ICR2, icr2);
            karch_lapic_write(LAPIC_ICR1, icr1 | APIC_ICR_DEST_ALL | vector);
            break;
    }

    return 1;
}

void karch_ioapic_enable_all() {
    
    // --> disable i8259 icmr.
    //   : this will enable IOAPIC all.
    karch_i8259_imcr_disable();
}

void karch_ioapic_write_redirector(
    void* ioapic_addr, uint32_t entry, uint32_t high, uint32_t low)
{
	karch_ioapic_write((uint32_t)ioapic_addr, (uint8_t) (IOAPIC_REDIR_TABLE + entry * 2 + 1), high);
	karch_ioapic_write((uint32_t)ioapic_addr, (uint8_t) (IOAPIC_REDIR_TABLE + entry * 2), low);
}

/**
 * edge mode `EOI` method.
 */
void karch_ioapic_eoi_edge(uint8_t) {
    karch_lapic_eoi();
}

void karch_ioapic_eoi_lv(uint8_t irq) {
    karch_ioapic_irq_t* target_irq = &ioapic_irq[irq];
    uint32_t tmr = karch_lapic_read(
        LAPIC_TMR + 0x10 * ((target_irq->vector) >> 5));

    // --> edge mode `EOI` first then,
    karch_ioapic_eoi_edge(irq);
    
    const uint32_t flag = (1 << ((target_irq->vector) & 0x1f));

    // --> delivered by edge trigger mode??
    //   : in anyway, if it delivered as edge trigger mode, 
    //   : it requires recovery workarounds for broken chipsets.

    if ((tmr & flag) == 0) { 
        karch_ioapic_t* ioapic = target_irq->apic;
        // --> was not for level trigger mode.
        //   : recover 

        uint32_t reg = IOAPIC_REDIR_TABLE + target_irq->pin * 2;
        uint32_t low = karch_ioapic_read(ioapic->addr, reg);
        uint32_t maskbit = low & APIC_ICR_INT_MASK;

        // --> set mask and edge trigger mode.
        low |= APIC_ICR_INT_MASK;
        low &= ~APIC_ICR_TRIGGER;

        karch_ioapic_write(ioapic->addr, reg, low);
    
        // --> set back to level and restore the mask bit.
        low |= APIC_ICR_TRIGGER;
        low &= ~APIC_ICR_INT_MASK;
        low |= maskbit;
        
        karch_ioapic_write(ioapic->addr, reg, low);
    }
}

void karch_ioapic_set_irq(uint8_t irq) {
    if (irq >= MAX_IOAPIC_IRQ) {
        return;
    }

    // --> translate to IOAPIC irq number.
    irq = ioapic_irq_rvo[irq];

    // --> already set.
    karch_ioapic_irq_t* target_irq = &ioapic_irq[irq];
    if (target_irq->apic && target_irq->eoi) {
        return;
    }

    for(uint8_t i = 0; i < ioapic_n; ++i) {
        karch_ioapic_t* apic = &ioapic[i];

        /* check IRQ range. */
        if (apic->gsi_base <= irq &&
            apic->gsi_base + apic->pins > irq) 
        {
            target_irq->apic = apic;
            target_irq->pin = irq - apic->gsi_base;
            target_irq->vector = APIC_HW_IRQ_VECTOR_OFFSET + irq;
            
            // --> set EOI method in here.
            if (irq < 16) {
                target_irq->eoi = karch_ioapic_eoi_edge;
            }
            else {
                target_irq->eoi = karch_ioapic_eoi_lv;
            }

            // --> legacy (E)ISA are reserved as `edge trigger`.
            uint32_t lo = 0, hi = ((uint32_t)lapic_bsp) << 24;
            if (irq >= 16) {
		        /* PCI active-low */
                lo |= APIC_ICR_INT_POLARITY;

                /* PCI level triggered */
                lo |= APIC_ICR_TRIGGER;
            }
            else {
                /* ISA active-high */
                /* ISA edge triggered */
            }

            lo |= target_irq->vector;
            karch_ioapic_write_redirector(
                (void*) apic->addr, target_irq->pin, hi, lo
            );

            break;
        }
    }
}

void karch_ioapic_disable_irq(uint8_t irq) {
    if (irq >= MAX_IOAPIC_IRQ) {
        return;
    }

    // --> translate to IOAPIC irq number.
    irq = ioapic_irq_rvo[irq];

    // --> already set.
    karch_ioapic_irq_t* target_irq = &ioapic_irq[irq];
    if (!target_irq->apic) {
        return;
    }

    karch_ioapic_disable_pin(target_irq->apic->addr, target_irq->pin);
    target_irq->state |= 1;
}

void karch_ioapic_enable_irq(uint8_t irq) {
    if (irq >= MAX_IOAPIC_IRQ) {
        return;
    }

    // --> translate to IOAPIC irq number.
    irq = ioapic_irq_rvo[irq];

    // --> already set.
    karch_ioapic_irq_t* target_irq = &ioapic_irq[irq];
    if (!target_irq->apic) {
        return;
    }

    karch_ioapic_enable_pin(target_irq->apic->addr, target_irq->pin);
    target_irq->state &= ~1;
}
