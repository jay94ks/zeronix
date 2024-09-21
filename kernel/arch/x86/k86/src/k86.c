#define __ARCH_X86_INTERNALS__ 1
#include <x86/k86.h>
#include <x86/klib.h>

#include <x86/boot/bootinfo.h>

#include <x86/k86/irq.h>
#include <x86/k86/tables.h>
#include <x86/k86/taskseg.h>
#include <x86/k86/paging.h>
#include <x86/k86/cpulocal.h>
#include <x86/k86/mmap.h>
#include <x86/tasks/task.h>

#include <x86/legacy/i8259.h>
#include <x86/legacy/except.h>

#include <x86/peripherals/acpi.h>
#include <x86/peripherals/apic.h>
#include <x86/peripherals/fpu.h>

#include <x86/spinlock.h>
#include <x86/k86/systick.h>
#include <zeronix/kstring.h>

// --
void karch_k86_load_segs(uint8_t n);

// --> defined at head.asm.
extern void jump_to_kmain();

// --
void karch_k86_init(bootinfo_t* info) {
    // --> initialize IRQ hooks.
    karch_irq_init();

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
    
    // --> shift to new kernel segment.
    karch_k86_load_segs(0);

    // --> re-initialize early paging.
    //   : after this call, bootstrap code is not needed anymore.
    karch_mmap_init(info);
    karch_paging_early_init(info);

    // --> initialize the ACPI.
    if (karch_acpi_init()) {
        karch_apic_init();
    }

}

/**
 * load kernel segments.
 * reused at smp.asm.
 */
void karch_k86_load_segs(uint8_t n) {
    // --> load LDT and setup TSS.
    load_ldt(SEG_SEL(GDT_LDT));
    load_tr(SEG_TSS(n));

    // --> shift to new kernel segment.
    load_kern_cs();
    load_ds(SEG_SEL(GDT_KERN_DS));
    load_es(SEG_SEL(GDT_KERN_DS));
    load_fs(SEG_SEL(GDT_KERN_DS));
    load_gs(SEG_SEL(GDT_KERN_DS));
    load_ss(SEG_SEL(GDT_KERN_DS));
}

/**
 * enter to `kmain`, this called in BSP CPU.
 */
void karch_k86_enter_kmain() {
    uint8_t now_id = karch_lapic_number();
    karch_stackmark_t* sm 
        = karch_taskseg_get_stackmark(now_id);

    // --
    karch_cpulocals_init();
    karch_systick_init();
    karch_fpu_init();

    // --
    switch_stack(sm, jump_to_kmain);
}
