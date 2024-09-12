#define __ARCH_X86_INTERNALS__

#include <zeronix/types.h>
#include <zeronix/boot.h>
#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/layout.h>
#include <zeronix/arch/x86/cpu.h>

#include "min86/page.h"
#include "min86/gdt.h"
#include "min86/tss.h"
#include "min86/idt.h"
#include "min86/i8259.h"
#include "min86/except.h"
#include "min86/i8253.h"

// --
#include "smp/acpi.h"
#include "smp/apic.h"
#include "smp/smp.h"

// --
extern void* _karch_tss0_stack;

// --
kbootinfo_t kinfo;
karch_cpu_t cpus[MAX_CPU] __aligned(8);
uint8_t cpu_n, mode_min86;


// --
void karch_early_init();

/**
 * arch-specific initialization. 
 * the arch'll be initialized to minimal protected x86 mode.
 * after the kernel is ready to enter SMP, it'll call `karch_smp_init()`.
 */
void karch_init(kbootinfo_t* info) {
    kmemcpy(&kinfo, info, sizeof(kinfo));
    kmemset(cpus, 0, sizeof(cpus));
    
    mode_min86 = 1;
    cpu_n = 1;

    karch_early_init();
    
    // --> initialize ACPI.
    if (karch_init_acpi() == 0 ||
        karch_init_apic() == 0 ||
        karch_init_smp() == 0)
    {
        // --> working as min86 mode.
        return;
    }

    mode_min86 = 1;
    // --> init SMP.
}

/**
 * prepare IDT/GDT, i8259 PIC, shift to new segment and early paging.
 */
void karch_early_init() {
    // --> initialize GDT and TSS.
    karch_init_gdt();
    karch_init_tss();

    // --> then, apply GDT entries here.
    karch_flush_gdt();

    // --> initialize IDT and program i8259.
    karch_init_idt();
    karch_init_i8259();
    karch_init_except();

    // --> then, apply IDT entries here.
    karch_flush_idt();

    // --> load LDT and setup TSS.
    load_ldt(SEG_SEL(GDT_LDT));
    load_tr(SEG_TSS(0));

    // --> shift to new kernel segment.
    load_kern_cs();
    load_ds(SEG_SEL(GDT_KERN_DS));
    load_es(SEG_SEL(GDT_KERN_DS));
    load_fs(SEG_SEL(GDT_KERN_DS));
    load_gs(SEG_SEL(GDT_KERN_DS));
    load_ss(SEG_SEL(GDT_KERN_DS));

    // --> re-initialize early paging.
    //   : after this call, bootstrap code is not needed anymore.
    karch_init_page(&kinfo);
}

uint8_t* karch_stacktop_for(uint8_t cpu) {
    if (cpu >= MAX_CPU) {
        return 0;
    }

    uint8_t* top = (uint8_t*) &_karch_tss0_stack;
    return top - (I686_PAGE_SIZE * cpu);
}

uint8_t karch_count_cpu() {
    return cpu_n;
}

karch_cpu_t* karch_get_cpu(uint8_t n) {
    if (n >= cpu_n) {
        return 0;
    }

    return &cpus[n];
}

uint8_t karch_set_count_cpu(uint8_t n) {
    if (n >= MAX_CPU) {
        return 0;
    }

    cpu_n = n;
    return 1;
}

uint8_t karch_is_bsp_cpu(uint8_t n) {
    if (n >= MAX_CPU || n >= cpu_n) {
        return 0;
    }

    if (mode_min86 || !karch_smp_supported()) {
        return n == 0 ? 1 : 0;
    }

    if ((cpus[n].flags & CPUFLAG_INIT_BSP) != 0) {
        return cpus[n].is_bsp;
    }

    return 0;
}

uint8_t karch_is_bsp_cpu_current() {
    if (mode_min86 || !karch_smp_supported()) {
        return 1;
    }

    uint8_t smp_cpu = karch_smp_cpuid_current();
    return karch_is_bsp_cpu(smp_cpu);
}