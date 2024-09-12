#define __ARCH_X86_INTERNALS__

#include "apic.h"
#include "acpi.h"
#include "../min86/tss.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/cpuinfo.h>
#include <zeronix/arch/x86/layout.h>
#include <zeronix/arch/x86/cpu.h>

// --
static uint8_t smp_a2c[255];    // --> APIC ID -> CPU ID.
uint8_t smp_avail;


// --> post init LAPIC parameters: defined at apic.c.
extern void karch_apic_set_bsp(uint8_t cpu_id, uint8_t lapic_id);

// --
void karch_smp_map_apic_id();

// --
uint8_t karch_init_smp() {
    kmemset(smp_a2c, 0, sizeof(smp_a2c));
    smp_avail = 0;
    
    if (!karch_apic_supported()) {
        return 0;
    }

    karch_smp_map_apic_id();
    uint8_t n = karch_count_cpu();

    // --> initialize all tss.
    for (uint8_t i = 0; i < n; ++i) {
        karch_setup_tss(i, karch_stacktop_for(i));
    }

    // --> set the BSP info.
    uint8_t apic_id = karch_lapic_id();
    uint8_t bsp_cpu = smp_a2c[apic_id];
    karch_apic_set_bsp(bsp_cpu, apic_id);

    // --> enable LAPIC of BSP CPU.
    if (!karch_lapic_enable(bsp_cpu)) {
        // TODO: rollback.
        karch_apic_set_bsp(0, 0);
        return 0;
    }

    // --> here, IOAPIC information already detected.

    // TODO: enable IOAPIC ALL.
    // TODO: APIC idt init.
    // TODO: IDT reload.

    // TODO: start aps here.
    return 1;
}

/**
 * map CPU ID to apic id.
 */
void karch_smp_map_apic_id() {
    karch_acpi_madt_iter_t iter;
    karch_acpi_madt_item_t* item;
    karch_acpi_madt_iterate(&iter, 0);

    uint8_t cpu = 0;
    while (karch_acpi_madt_next(&iter, &item)) {
        if (item->type != ACPIMADT_LAPIC) {
            continue;
        }
        
        karch_acpi_madt_lapic_t* now = (karch_acpi_madt_lapic_t*) item;
        uint8_t cpu_id = cpu++;

        karch_cpu_t* cpu_ = karch_get_cpu(cpu_id);

        if (!cpu_) {
            break;
        }

        smp_a2c[now->apic] = cpu_id;
    }
}

uint8_t karch_smp_supported() {
    return smp_avail;
}

uint8_t karch_smp_cpu_id() {
    if (karch_count_cpu() <= 1) {
        return 0;
    }

    return smp_a2c[karch_lapic_id()];
}
