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

/* get irq count. */
uint8_t karch_irq_count();

/* register an IRQ handler. */
uint8_t karch_irq_register(uint8_t n, karch_irq_t* irq);

/* unregister an IRQ handler. */
uint8_t karch_irq_unregister(karch_irq_t* irq);

/* mask an IRQ. */
void karch_irq_mask(uint8_t n);

/* unmask an IRQ. */
void karch_irq_unmask(uint8_t n);


#ifdef __cplusplus
}
#endif

#endif