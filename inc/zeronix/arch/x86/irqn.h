#ifndef __INC_ZERONIX_ARCH_X86_IRQN_H__
#define __INC_ZERONIX_ARCH_X86_IRQN_H__

/* max IRQ numbers. */
#define MAX_IRQ         256

/* max physical IRQ number. */
#define MAX_PHYS_IRQ    64

/* exception vector offset. */
#define IRQ_EXCEPTION_OFFSET    0x40

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

    /* Exception IRQs. */
    IRQN_EXC_DIVIDE         = 0 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_SINGLE_STEP    = 1 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_NMI            = 2 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_BREAKPOINT     = 3 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_OVERFLOW       = 4 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_BOUNDS         = 5 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_INVAL_OPCODE   = 6 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_COPR_NAVL      = 7 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_DOUBLE_FAULT   = 8 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_COPR_SEGOVER   = 9 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_INVAL_TASKSEG  = 10 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_SEGMENT        = 11 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_STACK_FAULT    = 12 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_PROTECTION     = 13 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_PAGE_FAULT     = 14 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_COPROC_ERROR   = 16 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_ALIGNMENT      = 17 + IRQ_EXCEPTION_OFFSET,
    IRQN_EXC_MACHINE_CHECK  = 18 + IRQ_EXCEPTION_OFFSET,

    /* APIC Special IRQs. */
    IRQN_SCHED      = 0xf1, // --> this will not be dispatched to anywhere.
    IRQN_CPU_HALT   = 0xf2,
    IRQN_SPURIOUS   = 0xff,
    IRQN_ERROR      = 0xfe,
    IRQN_SYSTICK    = 0xf0,
} karch_irqn_t;

#endif