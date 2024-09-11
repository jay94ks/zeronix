#include <zeronix/types.h>
#include <zeronix/boot.h>
#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/layout.h>

#include "min86/page.h"
#include "min86/gdt.h"
#include "min86/tss.h"
#include "min86/idt.h"
#include "min86/i8259.h"
#include "min86/except.h"
#include "min86/i8253.h"

#include "smp/acpi.h"

// --
extern void* _karch_tss0_stack;

// --
kbootinfo_t kinfo;

// --
void karch_early_init();

/**
 * arch-specific initialization. 
 * the arch'll be initialized to minimal protected x86 mode.
 * after the kernel is ready to enter SMP, it'll call `karch_smp_init()`.
 */
void karch_init(kbootinfo_t* info) {
    kmemcpy(&kinfo, info, sizeof(kinfo));
    karch_early_init();
    
    // --> initialize ACPI.
    if (karch_init_acpi() == 0 ||
        karch_init_apic() == 0)
    {
        // --> working as min86 mode.
        return;
    }

    // --> init SMP.
}

uint8_t* karch_stacktop_for(uint8_t cpu) {
    if (cpu >= MAX_CPU) {
        return 0;
    }

    uint8_t* top = (uint8_t*) &_karch_tss0_stack;
    return top - (I686_PAGE_SIZE * cpu);
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