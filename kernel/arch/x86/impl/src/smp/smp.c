#define __ARCH_X86_INTERNALS__  1

#include "apic.h"
#include "apic_idt.h"
#include "acpi.h"
#include "smp.h"
#include "../min86/tss.h"

#include <zeronix/boot.h>
#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/cpuinfo.h>
#include <zeronix/arch/x86/layout.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/cpu.h>

// --
static uint8_t smp_a2c[255];    // --> APIC ID -> CPU ID.
uint32_t smp_bios_rstv;         // --> bios reset vector.
uint8_t smp_avail;

// --
#define BIOS_RESET_VECTOR_ADDR  0x467
#define BIOS_RESET_VECTOR       *((volatile uint32_t*) BIOS_RESET_VECTOR_ADDR)

#define RTC_INDEX   0x70
#define RTC_IO      0x71

// --> extern from ../arch.c.
kbootinfo_t* karch_get_bootinfo();

// --> post init LAPIC parameters: defined at apic.c.
extern void karch_apic_set_bsp(uint8_t cpu_id, uint8_t lapic_id);

// --> defined in `head.asm`, this must be passed to `jmp` instruction.
extern void jump_to_kmain();

// --
void karch_smp_set_mode(uint8_t bsp_id);
void karch_smp_map_apic_id();
void karch_smp_start_ap();

// -- symbols to put data for CPUs.
extern void smp_ap_entry();         // --> AP entry.
extern void* __smp_ap_id;           // --> CPU ID
extern void* __smp_ap_pt;           // --> PAGE TABLE
extern void* __smp_ap_gdt;          // --> GDT descriptor
extern void* __smp_ap_idt;          // --> IDT descriptor  
extern void* __smp_ap_gdt_tab;      // --> GDT table.
extern void* __smp_ap_idt_tab;      // --> IDT table.
extern void* __smp_entry_end;       // --> end of SMP entry.

#define smp_ap_id       *((volatile uint32_t*) &__smp_ap_id)
#define smp_ap_pt       *((volatile uint32_t*) &__smp_ap_pt)
#define smp_ap_gdt       (*((karch_desc_t*) &__smp_ap_gdt))
#define smp_ap_idt       (*((karch_desc_t*) &__smp_ap_idt))
#define smp_ap_gdt_tab   (*((karch_seg_t*) &__smp_ap_gdt_tab))
#define smp_ap_idt_tab   (*((karch_gate_t*) &__smp_ap_idt_tab))
#define smp_ap_entry_beg ((uint32_t) &smp_ap_entry)
#define smp_ap_entry_end ((uint32_t) &__smp_entry_end)

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
    
    // --> set the SMP mode as BSP.
    bsp->smp_mode = CPUSMPF_BSP;
    bsp->flags |= CPUFLAG_INIT_SMP_MODE;

    // --> set other CPUs as CPUSMPF_NOT_BSP.
    karch_smp_set_mode(bsp_cpu);

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
 * set SMP mode for other CPUs.
 */
void karch_smp_set_mode(uint8_t bsp_id) {
    uint8_t n = karch_count_cpu();
    for(uint8_t i = 0; i < n; ++i) {
        if (i == bsp_id) {
            continue;
        }

        karch_cpu_t* cpu = karch_get_cpu(i);
        cpu->smp_mode = CPUSMPF_NOT_BSP;
        cpu->flags |= CPUFLAG_INIT_SMP_MODE;
    }
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

uint32_t karch_smp_roundup(uint32_t n, uint32_t v) {
    if ((n % v) != 0) {
        return (n / v + 1) * v;
    }

    return n;
}

/**
 * find lowest free memory.
 */
uint32_t karch_smp_find_lowmem(uint32_t len) {
    kbootinfo_t* boot = karch_get_bootinfo();
	uint32_t lowest = 0xffffffffu;
    uint32_t alloc = 0xffffffffu;

    len = karch_smp_roundup(len, I686_PAGE_SIZE);

    for(uint32_t i = 0; i < boot->mmap_cnt; ++i) {
        if (boot->mmap[i].len < len) {
            continue;
        }

        if (boot->mmap[i].addr < lowest) {
            lowest = boot->mmap[i].addr;
            alloc = i;
        }
    }

    // --> make selected range as un-available.
    if (alloc != 0xffffffffu) {
        boot->mmap[alloc].addr += len;
        boot->mmap[alloc].len -= len;
    }

	return (void*) lowest;
}

/**
 * prepare entry point for each APs.
 * returns 0xffffffffu if failed, otherwise, it is lowest address.
 */
uint32_t karch_smp_prepare_ap_entry() {
    uint32_t size = smp_ap_entry_end - smp_ap_entry_beg;
    uint32_t lowmem = karch_smp_find_lowmem(size);

    // --> address must be in 1MB.
    if (lowmem + size >= 0x00100000u) {
        return 0xffffffffu;
    }

#define smp_ap_id       *((volatile uint32_t*) &__smp_ap_id)
#define smp_ap_pt       *((volatile uint32_t*) &__smp_ap_pt)
#define smp_ap_gdt       (*((karch_desc_t*) &__smp_ap_gdt))
#define smp_ap_idt       (*((karch_desc_t*) &__smp_ap_idt))
#define smp_ap_gdt_tab   (*((karch_seg_t*) &__smp_ap_gdt_tab))
#define smp_ap_idt_tab   (*((karch_gate_t*) &__smp_ap_idt_tab))
#define smp_ap_entry_beg ((uint32_t) &smp_ap_entry)
#define smp_ap_entry_end ((uint32_t) &__smp_entry_end)
    /**
     * 
	memcpy(&__ap_gdt_tab, gdt, sizeof(gdt));
	memcpy(&__ap_idt_tab, gdt, sizeof(idt));
	__ap_gdt.base = ap_lin_addr(&__ap_gdt_tab);
	__ap_gdt.limit = sizeof(gdt)-1;
	__ap_idt.base = ap_lin_addr(&__ap_idt_tab);
	__ap_idt.limit = sizeof(idt)-1;
    */

   smp_ap_lower = lowmem;

   //smp_ap_gdt
    
    // --> copy ap entry fn itself.
    uint32_t copy_size = smp_ap_entry_end - smp_ap_entry_beg;
    kmemcpy((void*) lowmem, smp_ap_entry, size);

    return lowmem;
}

/**
 * start all APs in CPU.
 */
void karch_smp_start_ap() {
    karch_emergency_print("SMP: starting APs...");

    // --> copy bios reset vector.
    smp_bios_rstv = BIOS_RESET_VECTOR;
    
    // --> set bios shutdown code to 0x0a.
    cpu_out8(RTC_INDEX, 0x0f);
    cpu_out8(RTC_IO, 0x0a);

    uint32_t ap_entry = karch_smp_prepare_ap_entry();
    if (ap_entry == 0xffffffffu) {
        // kernel panic: no memory to prepare SMP entry for each APs.
        karch_emergency_print("fatal: no memory to prepare SMP entry for each APs.");
        while(1);
    }


    // TODO: run kmain on BSP.
    while(1);
}

/**
 * start the AP.
 */
void karch_smp_init_ap32() {

    while(1);
}