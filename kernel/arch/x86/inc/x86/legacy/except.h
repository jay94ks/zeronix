#ifndef __KERNEL_ARCH_X86_INC_X86_LEGACY_EXCEPT_H__
#define __KERNEL_ARCH_X86_INC_X86_LEGACY_EXCEPT_H__
#include <x86/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * exception numbers.
 */
typedef enum {
    EXCEPT_DIVIDE_ERROR = 0,
    EXCEPT_SINGLE_STEP  = 1,
    EXCEPT_NMI          = 2,
    EXCEPT_BREAKPOINT   = 3,
    EXCEPT_OVERFLOW     = 4,
    EXCEPT_BOUNDS       = 5,
    EXCEPT_INVAL_OPCODE = 6,
    EXCEPT_COPR_NAVL    = 7,    // --> co-processor not available.
    EXCEPT_DOUBLE_FAULT = 8,
    EXCEPT_COPR_SEGOVER = 9,
    EXCEPT_INVAL_TSS    = 10,
    EXCEPT_SEGMENT      = 11,    // --> segment not present.
    EXCEPT_STACK_FAULT  = 12,
    EXCEPT_GENERAL_PROT = 13,    // --> general protection fault.
    EXCEPT_PAGE_FAULT   = 14,
    EXCEPT_COPROC_ERROR = 16,
    EXCEPT_ALIGNMENT    = 17,
    EXCEPT_MACHINE_CHECK= 18
} karch_except_irq_t;

/**
 * exception parameter.
 * n: IRQ number, 0 ~ 15.
 * k: was kernel env or not.
 * frame: frame pointer.
 * ----
 * frame->ip, cs, flags : always valid.
 * frame->sp, ss: valid only if k == 0.
 */
typedef struct {
    karch_except_irq_t n;
    uint32_t k;
    karch_except_frame_t* frame;
    void* data;
} karch_except_t;

/**
 * A callback type to pass interrupt execution to other.
 */
typedef void (*karch_except_cb_t)(const karch_except_t* except);

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize the legacy exception handlers.
 */
void karch_except_init();
#endif

/**
 * get the exception interrupt handler.
 * returns non-zero if success.
 */
uint8_t karch_except_get_handler(karch_except_irq_t n, karch_except_cb_t* cb, void** data);

/**
 * set the exception interrupt handler.
 * returns non-zero if success.
 */
uint8_t karch_except_set_handler(karch_except_irq_t n, karch_except_cb_t cb, void* data);

#ifdef __cplusplus
}
#endif
#endif