#ifndef __INC_ZERONIX_ARCH_IRQ_H__
#define __INC_ZERONIX_ARCH_IRQ_H__
#include <zeronix/types.h>

#ifdef __ARCH_X86__
#include "x86/irqn.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * karch_irq_t.
 */
struct __karch_irq_t {
    struct __karch_irq_t* next;
    uint8_t n;

    /* handler. */
    void (* handler)(struct __karch_irq_t* irq);
};

typedef struct __karch_irq_t karch_irq_t;

/**
 * interrupt frame.
 * this is written for x86.
 * if under other system, it must wrap this once.
 */
struct __karch_irq_frame_t {
    uint32_t err;  // --> if zero, not an error.

    /* hardware defined. */
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;

    /* only for crossing priv. */
    uint32_t sp;
    uint32_t ss;
} __packed;

/* short hand. */
typedef struct __karch_irq_frame_t karch_irq_frame_t;

/**
 * karch IRQ device. 
 */
typedef struct {
    /* total IRQ count. */
    uint16_t count;

    /* register an IRQ handler. */
    uint8_t (*reg)(uint8_t n, karch_irq_t* irq);

    /* unregister an IRQ handler. */
    uint8_t (*unreg)(karch_irq_t* irq);

    /* mask an IRQ. */
    void (*mask)(uint8_t n);

    /* unmask an IRQ. */
    void (*unmask)(uint8_t n);

    /* get the current interrupt frame. */
    karch_irq_frame_t* (*get_frame)();
} karch_irqdev_t;


#ifdef __cplusplus
}
#endif

#endif