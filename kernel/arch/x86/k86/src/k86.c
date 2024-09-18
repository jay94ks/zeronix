#define __ARCH_X86_INTERNALS__ 1
#include <x86/k86.h>
#include <x86/klib.h>

#include <x86/boot/bootinfo.h>

#include <x86/k86/tables.h>
#include <x86/k86/taskseg.h>
#include <x86/k86/paging.h>
#include <x86/k86/mmap.h>

#include <x86/legacy/i8259.h>
#include <x86/legacy/except.h>

#include <x86/peripherals/acpi.h>
#include <x86/peripherals/apic.h>

#include <x86/spinlock.h>
#include <zeronix/kstring.h>

// --
void karch_k86_load_segs();

// --
void karch_k86_init(bootinfo_t* info) {
    // --> initialize GDT and TSS.
    karch_tables_init();
    karch_taskseg_init();

    // --> then, apply GDT entries here.
    karch_tables_flush_gdt();

    // --> initialize IDT and program i8259.
    karch_i8256_init();
    karch_except_init();

    // --> then, apply IDT entries here.
    karch_tables_flush_idt();
    
    // --> load LDT and setup TSS.
    load_ldt(SEG_SEL(GDT_LDT));
    load_tr(SEG_TSS(0));

    // --> shift to new kernel segment.
    karch_k86_load_segs();

    // --> re-initialize early paging.
    //   : after this call, bootstrap code is not needed anymore.
    karch_paging_early_init(info);
    karch_mmap_init(info);

    // --> initialize the ACPI.
    if (karch_acpi_init()) {
        karch_apic_init();
    }

}

/**
 * load kernel segments.
 * reused at smp.asm.
 */
void karch_k86_load_segs() {
    // --> shift to new kernel segment.
    load_kern_cs();
    load_ds(SEG_SEL(GDT_KERN_DS));
    load_es(SEG_SEL(GDT_KERN_DS));
    load_fs(SEG_SEL(GDT_KERN_DS));
    load_gs(SEG_SEL(GDT_KERN_DS));
    load_ss(SEG_SEL(GDT_KERN_DS));
}
