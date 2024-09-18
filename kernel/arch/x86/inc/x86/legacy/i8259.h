#ifndef __KERNEL_ARCH_X86_INC_X86_LEGACY_I8259_H__
#define __KERNEL_ARCH_X86_INC_X86_LEGACY_I8259_H__
#include <x86/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize the legacy i8259 interrupt controller.
 */
void karch_i8256_init();

/**
 * program the i8259 interrupt controller.
 * note that, `karch_i8256_init` calls this.
 */
void karch_i8259_program();

/**
 * setup IDT elements to IDT table.
 * note that, `karch_i8256_init` calls this.
 */
void karch_i8259_setup_idt();
#endif

/**
 * unmask specific IRQ vector.
 */
void karch_i8259_unmask(uint8_t irq);

/**
 * mask specific IRQ vector.
 */
void karch_i8259_mask(uint8_t irq);

/**
 * disable i8259 interrupt controller.
 */
void karch_i8259_disable();

/**
 * emit i8295 end of interrupt.
 */
void karch_i8259_eoi(uint8_t n);

/**
 * returns whether i8259's IMCR disabled or not.
 */
uint8_t karch_i8259_check_imcr();

#ifdef __ARCH_X86_INTERNALS__
/**
 * disconnect i8259 from system.
 */
void karch_i8259_imcr_disable();

/**
 * re-connect i8259 into system.
 */
void karch_i8259_imcr_enable();
#endif

#ifdef __cplusplus
}
#endif
#endif
