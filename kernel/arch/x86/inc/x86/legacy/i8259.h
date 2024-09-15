#ifndef __KERNEL_ARCH_X86_INC_X86_LEGACY_I8259_H__
#define __KERNEL_ARCH_X86_INC_X86_LEGACY_I8259_H__
#include <x86/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * IRQ numbers.
 */
typedef enum {
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
} karch_i8259_irq_t;

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
    karch_i8259_irq_t n;
    uint32_t k;
    karch_intr_frame_t* frame;
    void* data;
} karch_i8259_t;

/**
 * A callback type to pass interrupt execution to other.
 */
typedef void (*karch_i8259_cb_t)(const karch_i8259_t* i8259);

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
void karch_i8259_unmask(karch_i8259_irq_t irq);

/**
 * mask specific IRQ vector.
 */
void karch_i8259_mask(karch_i8259_irq_t irq);

/**
 * disable i8259 interrupt controller.
 */
void karch_i8259_disable();

/**
 * emit i8295 end of interrupt.
 */
void karch_i8259_eoi(karch_i8259_irq_t n);

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

/**
 * get the i8259 interrupt handler for specified irq.
 * returns non-zero if success.
 */
uint8_t karch_i8259_get_handler(karch_i8259_irq_t n, karch_i8259_cb_t* cb, void** data);

/**
 * set the i8259 interrupt handler for specified irq.
 * returns non-zero if success.
 */
uint8_t karch_i8259_set_handler(karch_i8259_irq_t n, karch_i8259_cb_t cb, void* data);

#ifdef __cplusplus
}
#endif
#endif
