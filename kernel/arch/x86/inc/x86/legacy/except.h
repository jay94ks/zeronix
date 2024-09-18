#ifndef __KERNEL_ARCH_X86_INC_X86_LEGACY_EXCEPT_H__
#define __KERNEL_ARCH_X86_INC_X86_LEGACY_EXCEPT_H__
#include <x86/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * exception numbers.
 * --> merged to IRQN_EXC_*.
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
 */

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize the legacy exception handlers.
 */
void karch_except_init();
#endif

#ifdef __cplusplus
}
#endif
#endif