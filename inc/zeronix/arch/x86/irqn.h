#ifndef __INC_ZERONIX_ARCH_X86_IRQN_H__
#define __INC_ZERONIX_ARCH_X86_IRQN_H__

/* max IRQ numbers. */
#define MAX_IRQ         256

/* max physical IRQ number. */
#define MAX_PHYS_IRQ    64

/**
 * IRQ numbers.
 */
typedef enum {
    IRQN_TIMER = 0,
    IRQN_KEYBOARD = 1,
    IRQN_SLAVE = 2,
    IRQN_COM24 = 3,
    IRQN_COM13 = 4,
    IRQN_LPT2 = 5,
    IRQN_FLOPPY = 6,
    IRQN_LPT1 = 7, // --> unreliable "spurious" (from OS dev, https://wiki.osdev.org/Interrupts)
    IRQN_RTC = 8,
    IRQN_MOUSE = 12,
    IRQN_MATH = 13,
    IRQN_HDD1 = 14,
    IRQN_HDD2 = 15,

    /* APIC Special IRQs. */
    IRQN_SCHED      = 0xf1,
    IRQN_CPU_HALT   = 0xf2,
    IRQN_SPURIOUS   = 0xff,
    IRQN_ERROR      = 0xfe,
    IRQN_SYSTICK    = 0xf0,
} karch_irqn_t;

#endif