#define __ARCH_X86_INTERNALS__  1

#include "apic.h"
#include "apic_idt.h"
#include "acpi.h"
#include "smp.h"
#include "../min86/tss.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/cpuinfo.h>
#include <zeronix/arch/x86/layout.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/cpu.h>

// --
static uint8_t smp_a2c[255];    // --> APIC ID -> CPU ID.
uint8_t smp_avail;

// --> post init LAPIC parameters: defined at apic.c.
extern void karch_apic_set_bsp(uint8_t cpu_id, uint8_t lapic_id);

// --> defined in `head.asm`, this must be passed to `jmp` instruction.
extern void jump_to_kmain();

// --
void karch_smp_map_apic_id();
void karch_smp_start_ap();

// --
uint8_t karch_init_smp() {
    kmemset(smp_a2c, 0, sizeof(smp_a2c));
    smp_avail = 0;
    
    if (!karch_apic_supported()) {
        return 0;
    }

    karch_emergency_print("SMP: init...");
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

    // --> set SMP available.
    smp_avail = 1;

    // --> enable LAPIC of BSP CPU.
    if (!karch_lapic_enable(bsp_cpu)) {
        smp_avail = 0;

        // TODO: rollback.
        karch_apic_set_bsp(0, 0);
        karch_set_count_cpu(1);

        karch_deinit_apic_idt();
        return 0;
    }

    // --> get BSP cpu info.
    karch_cpu_t* bsp = karch_get_cpu(bsp_cpu);
    
    // --> disable i8259 icmr.
    //   : this will enable IOAPIC all.
    karch_i8259_imcr_disable();

    // --> load APIC's IDT to IDT table.
    karch_init_apic_idt();

    // --> then, flush IDT table to CPU.
    karch_flush_idt();

    // --> switch kernel stack to.
    switch_stack(bsp->stackmark, karch_smp_start_ap);

    // --> can not reach here.
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

uint8_t karch_smp_cpuid(uint8_t lapic_id) {
    if (karch_count_cpu() <= 1) {
        return 0;
    }

    return smp_a2c[lapic_id];
}

uint8_t karch_smp_cpuid_current() {
    if (karch_count_cpu() <= 1) {
        return 0;
    }

    return karch_smp_cpuid(karch_lapic_id());
}

/**
 * start all APs in CPU.
 */
void karch_smp_start_ap() {
    karch_emergency_print("SMP: starting APs...");

    // TODO: run kmain on BSP.
    while(1);
}