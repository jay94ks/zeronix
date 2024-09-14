#define __ARCH_X86_INTERNALS__  1

#include "apic.h"
#include "apic_idt.h"
#include "apic_ipi.h"
#include "acpi.h"
#include "smp.h"
#include "../min86/tss.h"
#include "../min86/idt.h"
#include "../min86/gdt.h"

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
int16_t smp_ap_probe;           // --> probe to detect AP's boot completion.

// --
#define BIOS_RESET_VECTOR_ADDR  0x467
#define BIOS_RESET_VECTOR       *((volatile uint32_t*) BIOS_RESET_VECTOR_ADDR)

#define RTC_INDEX   0x70
#define RTC_IO      0x71

#define SMP_AP_ENTRY_COPY_ADDR  0x8000
#define SMP_WAIT_CPU_MDELAY     (60 * 1000)

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
void karch_smp_init_ap32();
void karch_smp_identify_cpu(uint8_t cpu_id);

// -- symbols to put data for CPUs.
extern void smp_ap_entry();         // --> AP entry.
extern void* __smp_ap_id;           // --> CPU ID
extern void* __smp_ap_pt;           // --> PAGE TABLE
extern void* __smp_ap_gdt;          // --> GDT descriptor
extern void* __smp_ap_idt;          // --> IDT descriptor  
extern void* __smp_ap_gdt_sys;      // --> GDT descriptor
extern void* __smp_ap_idt_sys;      // --> IDT descriptor  
extern void* __smp_ap_gdt_tab;      // --> GDT table.
extern void* __smp_ap_idt_tab;      // --> IDT table.
extern void* __smp_entry_end;       // --> end of SMP entry.

#define smp_ap_base     *((volatile uint32_t*) &__smp_ap_base)
#define smp_ap_id       *((volatile uint32_t*) &__smp_ap_id)
#define smp_ap_pt       *((volatile uint32_t*) &__smp_ap_pt)
#define smp_ap_gdt       (*((karch_desc_t*) &__smp_ap_gdt))
#define smp_ap_idt       (*((karch_desc_t*) &__smp_ap_idt))
#define smp_ap_gdt_sys   (*((karch_desc_t*) &__smp_ap_gdt_sys))
#define smp_ap_idt_sys   (*((karch_desc_t*) &__smp_ap_idt_sys))
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
    uint32_t lowmem = SMP_AP_ENTRY_COPY_ADDR; //karch_smp_find_lowmem(size);

    // --> address must be in 1MB.
    if (lowmem + size >= 0x00100000u) {
        return 0xffffffffu;
    }

    karch_desc_t* idt = karch_get_idt_ptr();
    karch_desc_t* gdt = karch_get_gdt_ptr();
    kbootinfo_t* boot = karch_get_bootinfo();

    smp_ap_id = 0;
    smp_ap_pt = (uint32_t) boot->pagedir; // --> share BSP's page table.
    smp_ap_gdt.base = (((uint32_t*)&smp_ap_gdt_tab) - ((uint32_t) &smp_ap_entry)) + lowmem;
    smp_ap_gdt.limit = gdt->limit;
    smp_ap_idt.base = (((uint32_t*)&smp_ap_idt_tab) - ((uint32_t) &smp_ap_entry)) + lowmem;
    smp_ap_idt.limit = idt->limit;

    smp_ap_gdt_sys = *gdt;
    smp_ap_idt_sys = *idt;

    kmemcpy(&smp_ap_idt_tab, (uint8_t*)idt->base, idt->limit + 1);
    kmemcpy(&smp_ap_gdt_tab, (uint8_t*)gdt->base, gdt->limit + 1);

    // --> copy ap entry fn itself.
    uint32_t copy_size = smp_ap_entry_end - smp_ap_entry_beg;
    kmemcpy((void*) lowmem, &smp_ap_entry, copy_size);

    return lowmem;
}

/**
 * start all APs in CPU.
 */
void karch_smp_start_ap() {
    karch_emergency_print("SMP: starting APs...");
    uint8_t cpu_n = karch_count_cpu();
    uint8_t bsp_id = karch_smp_cpuid_current();

    // --> backup bios reset vector.
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

    BIOS_RESET_VECTOR = ap_entry; // --> now, APs will boot here.

    // --> AP's ID pointer that points current ID value.
    uint32_t* ap_id = (uint32_t*) (ap_entry + ((uint32_t)&smp_ap_id - (uint32_t)&smp_ap_entry));
    for (uint8_t i = 0; i < cpu_n; ++i) {
        if (bsp_id == i) {  // --> skip boot CPU.
            continue;
        }

        // --> set the CPU ID.
        const uint8_t now_id = i;
        *ap_id = smp_ap_id = i;
        
        karch_cpu_t* cpu = karch_get_cpu(now_id);

        // --> reset the probe to wait boot completion state.
        smp_ap_probe = -1;
        cpu_mfence();

        if (!karch_lapic_send_init_ipi(now_id)) {
            // --> failed to send init command.
            continue;
        }

        if (!karch_lapic_send_startup_ipi(now_id, ap_entry)) {
            // --> failed to send startup command.
            continue;
        }

        uint32_t counter = 0;
        while (counter++ < SMP_WAIT_CPU_MDELAY) { // --> 60 seconds.
            // --> delay 10 millis.
            karch_lapic_mdelay(1); 

            // --> CPU is ready now.
            if (smp_ap_probe == now_id) {
                cpu->flags |= CPUFLAG_INIT_SMP_BOOT;
                break;
            }
        }
    }

    // --> restore bios reset vector here.
    BIOS_RESET_VECTOR = smp_bios_rstv;

    // --> set bios shutdown code to 0x00.
    cpu_out8(RTC_INDEX, 0x0f);
    cpu_out8(RTC_IO, 0x00);

    karch_smp_identify_cpu(bsp_id);
    karch_cpu_t* bsp = karch_get_cpu(bsp_id);
    if (bsp) {
        bsp->flags |= CPUFLAG_INIT_SMP_BOOT;
    }

    // TODO: run kmain on BSP.
    
    while(1);
}

/**
 * called from `karch_smp_startup_ap32`. (<-- smp_ap_entry)
 */
void karch_smp_boot_ap32() {
    karch_cpu_t* cpu = karch_get_cpu(smp_ap_id);
    if (!cpu) {
        // --> AP panic.
        while(1) {
            cpu_hlt();
        }

        return;
    }

    // --> switch stack space, then continue to `karch_smp_init_ap32`.
    switch_stack(cpu->stackmark, karch_smp_init_ap32);
}

/**
 * start the AP.
 */
void karch_smp_init_ap32() {
    uint32_t now_id = smp_ap_id;
    uint16_t* vga = (uint16_t*) 0xb8000;

    // --> set completion here.
    smp_ap_probe = now_id;
    cpu_mfence();

    // --> identify current running AP.
    karch_smp_identify_cpu(now_id);

    *(vga + (now_id + 1)) = ('0' + now_id) | (15 << 8);
    while(1);
}

void karch_smp_identify_cpu(uint8_t cpu_id) {
    karch_cpu_t* cpu = karch_get_cpu(cpu_id);
    if (!cpu) {
        return;
    }

    uint32_t eax = 0, ebx, edx, ecx;
    read_cpuid(&eax, &ebx, &ecx, &edx);

    if (ebx == INTEL_CPUID_GEN_EBX && 
        ecx == INTEL_CPUID_GEN_ECX &&
        edx == INTEL_CPUID_GEN_EDX) 
    {
        // intel.
    }

    else if (
        ebx == AMD_CPUID_GEN_EBX && 
        ecx == AMD_CPUID_GEN_ECX &&
        edx == AMD_CPUID_GEN_EDX) 
    {
        // amd.
    }

    else {
        // unknown.
    }

    if (eax == 0) {
        return;
    }
    
    eax = 1;
    read_cpuid(&eax, &ebx, &ecx, &edx);

    uint32_t stepping = eax & 0x0f;
    uint32_t model = (eax >> 4) & 0x0f;

    if (model == 0x0f || model == 0x06) {
        model += ((eax >> 16) & 0x0f) << 4;
    }

    uint32_t family = (eax >> 8) & 0x0f;
    if (family == 0x0f) {
        family += (eax >> 20) & 0xff;
    }

    cpu->ident_step = stepping;
    cpu->ident_model = model;
    cpu->ident_family = family;
    cpu->ident_ecx = ecx;
    cpu->ident_edx = edx;
}