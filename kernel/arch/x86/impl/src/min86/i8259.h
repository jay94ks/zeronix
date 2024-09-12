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
 * IRQ numbers.
 */
enum {
    I8259_TIMER = 0,
    I8259_KEYBOARD = 1,
    I8259_SLAVE = 2,
    I8259_COM24 = 3,
    I8259_COM13 = 4,
    I8259_LPT2 = 5,
    I8259_FLOPPY = 6,
    I8259_LPT1 = 7, // --> unreliable "spurious" (from OS dev, https://wiki.osdev.org/Interrupts)
    I8259_RTC = 8,
    I8259_MOUSE = 12,
    I8259_MATH = 13,
    I8259_HDD1 = 14,
    I8259_HDD2 = 15
};

/**
 * callback parameter.
 * n: IRQ number, 0 ~ 15.
 * k: was kernel env or not.
 * frame: frame pointer.
 * ----
 * frame->ip, cs, flags : always valid.
 * frame->sp, ss: valid only if k == 0.
 */
typedef struct {
    uint32_t n;
    uint32_t k;
    karch_intr_frame_t* frame;
    void* data;
} i8259_t;

/**
 * A callback type to pass interrupt execution to other.
 */
typedef void (*karch_i8259_cb_t)(const i8259_t* i8259);

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
 * get the interrupt handler for irq N.
 * returns non-zero if success.
 */
uint8_t karch_get_hwint_i8259(uint8_t n, karch_i8259_cb_t* cb, void** data);

/**
 * set the interrupt handler for irq N.
 * returns non-zero if success.
 */
uint8_t karch_set_hwint_i8259(uint8_t n, karch_i8259_cb_t cb, void* data);

/**
 * disconnect i8259 from system.
 */
void karch_i8259_imcr_disable();

/**
 * re-connect i8259 into system.
 */
void karch_i8259_icmr_enable();

#endif