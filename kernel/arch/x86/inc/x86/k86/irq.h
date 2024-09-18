#ifndef __KERNEL_ARCH_X86_INC_X86_K86_IRQ_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_IRQ_H__
#include <zeronix/arch/irq.h>

#ifdef __cplusplus
extern "C" {
#endif

/* max IRQ numbers. */
#define MAX_IRQ 64

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize IRQ hookers.
 */
void karch_irq_init();

/**
 * dispatch an IRQ to hookers.
 */
void karch_irq_dispatch(uint8_t n);
#endif

#ifdef __cplusplus
}
#endif
#endif