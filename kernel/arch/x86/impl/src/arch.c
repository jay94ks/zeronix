#include <zeronix/types.h>
#include <zeronix/boot.h>
#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>

#include "min86/page.h"
#include "min86/gdt.h"
#include "min86/tss.h"
#include "min86/idt.h"
#include "min86/i8259.h"
#include "min86/except.h"
#include "min86/i8253.h"

// --
kbootinfo_t kinfo;
uint8_t i = 0;

void karch_timer_test(const i8256_t* info) {
    uint8_t n = i++;
    char ch = 'a' + n % ('z' - 'a');

    uint16_t* vga = (uint16_t* ) 0xb8000;
    *vga = ch | (15 << 8);
}

/**
 * arch-specific initialization. 
 */
void karch_init(kbootinfo_t* info) {
    kmemcpy(&kinfo, info, sizeof(kinfo));

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

    // --> re-initialize early paging.
    //   : after this call, bootstrap code is not needed anymore.
    karch_init_page(&kinfo);

    // --> set hw interrupt.
    karch_set_hwint_i8259(I8259_TIMER, karch_timer_test, 0);
    karch_unmask_i8259(I8259_TIMER);

    cpu_sti();

    // --> 10 tick per second.
    karch_init_i8253(6000);
    while(1);
}

