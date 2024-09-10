#define __NO_EXTERN_LAPIC_VARS__ 1

#include "apic.h"
#include "acpi.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/cpuinfo.h>

// --
karch_ioapic_t ioapic[MAX_IOAPIC];
karch_ioapic_irq_t ioapic_irq[MAX_IOAPIC_IRQ];
uint8_t ioapic_n;

// --
uint8_t apic_supported;
karch_lapic_info_t lapic_info;

// --
void karch_apic_collect();
// --

uint8_t karch_init_apic() {
    kmemset(ioapic, 0, sizeof(ioapic));
    kmemset(ioapic_irq, 0, sizeof(ioapic_irq));
    kmemset(&lapic_info, 0, sizeof(lapic_info));

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
    lapic_info.bsp_cpu_id = 0;
    lapic_info.bsp_lapic_id = 0;
    lapic_info.eoi_addr = 0;
    return 0;
}

/**
 * post init, called from smp.c, karch_init_smp.
 */
void karch_apic_set_bsp(uint8_t cpu_id, uint8_t lapic_id) {
    lapic_info.bsp_cpu_id = cpu_id;
    lapic_info.bsp_lapic_id = lapic_id;
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
}

uint32_t karch_lapic_id() {
    return lapic_read(LAPIC_ID) >> 24;
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

uint32_t karch_lapic_read(uint32_t iobase) {
    return *((volatile uint32_t*)(iobase));
}

void karch_lapic_write(uint32_t iobase, uint32_t val) {
    do { 
        *((volatile uint32_t*)(iobase)) = val;
    }
    while(0);
}

uint32_t karch_ioapic_enable_pin(uint32_t ioapic_addr, uint32_t pin) {
    uint32_t lo = karch_ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);
	karch_ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo & ~APIC_ICR_INT_MASK);
}

uint32_t karch_ioapic_disable_pin(uint32_t ioapic_addr, uint32_t pin) {
    uint32_t lo = karch_ioapic_read(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2);
	karch_ioapic_write(ioapic_addr, IOAPIC_REDIR_TABLE + pin * 2, lo | APIC_ICR_INT_MASK);
}