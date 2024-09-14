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
kbootinfo_t bootinfo;
uint8_t mode_min86;

// --
void karch_early_init();
void karch_load_segs();
void karch_set_min86();
void karch_init_cpu();

/**
 * arch-specific initialization. 
 * the arch'll be initialized to minimal protected x86 mode.
 * after the kernel is ready to enter SMP, it'll call `karch_smp_init()`.
 */
void karch_init(kbootinfo_t* info) {
    kmemcpy(&bootinfo, info, sizeof(bootinfo));
    karch_init_cpu();
    mode_min86 = 1;
    

    karch_early_init();
    mode_min86 = 0;

    // --> initialize ACPI.
    if (!karch_init_acpi() || !karch_init_apic())
    {
        // --> working as min86 mode.
        mode_min86 = 1;
        karch_set_min86();
        return;
    }

    if (karch_init_smp() != 0) {
        cpu_cli();

        // kernel panic: unreachable if SMP successfully initialized.
        karch_emergency_print(
            "fatal: the CPU reached to impossible point in SMP initialization.");

        while(1);
    }

    mode_min86 = 1;
    karch_set_min86();
}

kbootinfo_t* karch_get_bootinfo() {
    return &bootinfo;
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
    karch_load_segs();

    // --> re-initialize early paging.
    //   : after this call, bootstrap code is not needed anymore.
    karch_init_page(&bootinfo);
}

/**
 * load kernel segments.
 * reused by SMP's AP bootstrap in smp.asm.
 */
void karch_load_segs() {
    // --> shift to new kernel segment.
    load_kern_cs();
    load_ds(SEG_SEL(GDT_KERN_DS));
    load_es(SEG_SEL(GDT_KERN_DS));
    load_fs(SEG_SEL(GDT_KERN_DS));
    load_gs(SEG_SEL(GDT_KERN_DS));
    load_ss(SEG_SEL(GDT_KERN_DS));
}

void karch_set_min86() {
    karch_cpu_t* bsp = karch_get_cpu_current();
    if (!bsp) {
        cpu_cli();

        // kernel panic: unreachable if SMP successfully initialized.
        karch_emergency_print(
            "fatal: the kernel arch can not determines BSP for min86 mode.");

        while(1);
    }

    // --> set SMP mode and flag.
    bsp->smp_mode = CPUSMPF_NOT_SMP;
    bsp->flags |= CPUFLAG_INIT_SMP_MODE;
}

void karch_emergency_print(const char* msg) {
    uint16_t* vba = (uint16_t*) 0xb8000;
    const char* msg_ = msg;

    while(*msg_) {
        *vba++ = (*msg_++) | (15 << 8);
    }

    *vba++ = '.' | (15 << 8);
    *vba++ = (15 << 8);
}
