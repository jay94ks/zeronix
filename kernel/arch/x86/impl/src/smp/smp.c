#include "apic.h"
#include "acpi.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/cpuinfo.h>
#include <zeronix/arch/x86/layout.h>

// --
static uint8_t smp_a2c[255];    // --> APIC ID -> CPU ID.
uint8_t smp_c2a[MAX_CPU];       // --> CPU ID -> APIC ID.
uint8_t smp_cpu;

// --> post init LAPIC parameters: defined at apic.c.
extern void karch_apic_set_bsp(uint8_t cpu_id, uint8_t lapic_id);

// --
void karch_smp_discover_cpu();

// --
uint8_t karch_init_smp() {
    kmemset(smp_a2c, 0, sizeof(smp_a2c));
    kmemset(smp_c2a, 0, sizeof(smp_c2a));
    smp_cpu = 0;

    if (!karch_apic_supported()) {
        smp_cpu = 1;
        return 0;
    }

    karch_smp_discover_cpu();

    // --> if no cpu found in anywhere.
    if (smp_cpu <= 0) {
        smp_cpu = 0;
        return 0;
    }

    // TODO: init all TSS.

    // --> set the BSP info.
    uint8_t apic_id = karch_lapic_id();
    karch_apic_set_bsp(smp_a2c[apic_id], apic_id);

    // TODO: enable LAPIC here.
    // --> here, IOAPIC information already detected.

    // TODO: enable IOAPIC ALL.
    // TODO: APIC idt init.
    // TODO: IDT reload.

    // TODO: start aps here.
    return 1;
}

/**
 * this finds all CPU in system using ACPI.
 */
void karch_smp_discover_cpu() {
    karch_acpi_madt_iter_t iter;
    karch_acpi_madt_item_t* item;
    karch_acpi_madt_iterate(&iter, 0);

    while (karch_acpi_madt_next(&iter, &item)) {
        if (item->type != ACPIMADT_LAPIC) {
            continue;
        }
        
        // --> map all LAPIC to CPU numbers.
        karch_acpi_madt_lapic_t* now = (karch_acpi_madt_lapic_t*) item;
        uint8_t cpu_id = smp_cpu++;
        
        smp_a2c[now->apic] = cpu_id;
        smp_c2a[cpu_id] = now->apic;
    }
}

uint8_t karch_smp_supported() {
    return smp_cpu > 1;
}

uint8_t karch_smp_cpu_id() {
    if (smp_cpu <= 1) {
        return 0;
    }

    return smp_a2c[karch_lapic_id()];
}
