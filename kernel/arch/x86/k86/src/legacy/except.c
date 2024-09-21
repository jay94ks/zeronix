#define __ARCH_X86_INTERNALS__ 1
#include <x86/legacy/except.h>
#include <x86/k86/irq.h>
#include <x86/k86/tables.h>
#include <x86/klib.h>

#include <zeronix/kstring.h>

#if 1
extern void karch_except_00();
extern void karch_except_01();
extern void karch_except_02();
extern void karch_except_03();
extern void karch_except_04();
extern void karch_except_05();
extern void karch_except_06();
extern void karch_except_07();
extern void karch_except_08();
extern void karch_except_09();
extern void karch_except_10();
extern void karch_except_11();
extern void karch_except_12();
extern void karch_except_13();
extern void karch_except_14();
extern void karch_except_16();
extern void karch_except_17();
extern void karch_except_18();
#endif

#define MAX_EXCEPT_CB   19

// --
static const struct gatevec gv_except[] = {
    { karch_except_00,  0, PRIV_KERN }, // --> divide error
    { karch_except_01,  1, PRIV_KERN }, // --> debug vector, single step.
    { karch_except_02,  2, PRIV_KERN }, // --> nmi.
    { karch_except_03,  3, PRIV_USER }, // --> breakpoint.
    { karch_except_04,  4, PRIV_USER }, // --> overflow
    { karch_except_05,  5, PRIV_KERN }, // --> bounds
    { karch_except_06,  6, PRIV_KERN }, // --> inval opcode
    { karch_except_07,  7, PRIV_KERN }, // --> copr not avail.
    { karch_except_08,  8, PRIV_KERN }, // --> double fault.
    { karch_except_09,  9, PRIV_KERN }, // --> coproc seg overflow.
    { karch_except_10, 10, PRIV_KERN }, // --> invalid tss.
    { karch_except_11, 11, PRIV_KERN }, // --> segment not present.
    { karch_except_12, 12, PRIV_KERN }, // --> stack fault.
    { karch_except_13, 13, PRIV_KERN }, // --> general protection fault.
    { karch_except_14, 14, PRIV_KERN }, // --> page fault.

    { karch_except_16, 16, PRIV_KERN }, // --> coproc error.
    { karch_except_17, 17, PRIV_KERN }, // --> alignment check.
    { karch_except_18, 18, PRIV_KERN }, // --> machine check.

    GATEVEC_NULL
};


// --
void karch_except_init() {
    // --> map exception handler vectors.
    karch_tables_load_idt(gv_except, 0);
}

void karch_except(uint32_t n, uint32_t k, karch_intr_frame_t* frame, karch_intr_regs_t* regs) {
    karch_irq_intr_begin(frame);

    uint8_t irqn = n + IRQ_EXCEPTION_OFFSET;
    karch_irq_dispatch(irqn);

    karch_irq_intr_end();
}