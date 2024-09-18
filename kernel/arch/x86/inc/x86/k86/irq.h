#ifndef __KERNEL_ARCH_X86_INC_X86_K86_IRQ_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_IRQ_H__
#include <zeronix/arch/irq.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize IRQ hookers.
 */
void karch_irq_init();

/**
 * dispatch an IRQ to hookers.
 */
void karch_irq_dispatch(uint8_t n);

/**
 * mask an IRQ without any overrides.
 */
void karch_irq_mask_phys(uint8_t n);

/**
 * unmask an IRQ without any overrides.
 */
void karch_irq_unmask_phys(uint8_t n);

/**
 * irq hardware overrides.
 */
typedef struct {
    void(* mask)(uint8_t n);
    void(* unmask)(uint8_t n);
} karch_irq_ovr_t;

/**
 * get the hardware overrides.
 */
karch_irq_ovr_t* karch_irq_get_override(uint8_t n);

/**
 * set the hardware overrides.
 */
void karch_irq_set_override(uint8_t n, karch_irq_ovr_t* ovr);
#endif

#ifdef __cplusplus
}
#endif
#endif