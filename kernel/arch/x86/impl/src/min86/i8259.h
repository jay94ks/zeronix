#ifndef __ZERONIX_ARCH_X86_LEGACY_i8259_H__
#define __ZERONIX_ARCH_X86_LEGACY_i8259_H__

#include <zeronix/types.h>
#include <zeronix/arch/x86/types.h>

/* 8259A interrupt controller ports. */
#define i8259_INT_CTL         0x20	/* I/O port for interrupt controller */
#define i8259_INT_CTLMASK     0x21	/* setting bits in this port disables ints */
#define i8259_INT2_CTL        0xA0	/* I/O port for second interrupt controller */
#define i8259_INT2_CTLMASK    0xA1	/* setting bits in this port disables ints */

/* Magic numbers for interrupt controller. */
#define i8259_END_OF_INT      0x20	/* code used to re-enable after an interrupt */

/**
 * A callback type to pass interrupt execution to other.
 * n: IRQ number, 0 ~ 15.
 * k: was kernel env or not.
 * frame: frame pointer.
 * ----
 * frame->ip, cs, flags : always valid.
 * frame->sp, ss: valid only if k == 0.
 */
typedef void (*karch_i8259_cb_t)(uint32_t n, uint32_t k, karch_intr_frame_t* frame);

/**
 * initialize i8259, but this does not enable i8259.
 * call karch_unmask_i8259() to enable it.
 * ----
 * this just load IDT vectors for i8259 chip.
 */
void karch_init_i8259();

/**
 * unmask the specified irq line.
 */
void karch_unmask_i8259(uint8_t irq);

/**
 * mask the specified irq line.
 */
void karch_mask_i8259(uint8_t irq);

/**
 * disable i8259.
 */
void karch_disable_i8259();

/**
 * set the interrupt handler for irq N.
 * and return the previous handler if set.
 */
karch_i8259_cb_t karch_set_hwint_i8259(uint8_t n, karch_i8259_cb_t cb);
#endif