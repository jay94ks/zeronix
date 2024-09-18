#ifndef __KERNEL_ARCH_X86_INC_X86_K86_SYSTICK_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_SYSTICK_H__

#include <zeronix/arch/arch.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_X86_INTERNALS__
#ifndef __NO_SYSTICK_EXTERNS__
extern uint32_t systick_freq;
#endif

/**
 * initialize the `SYSTICK` for BSP cpu.
 */
void karch_systick_init();

#endif

#ifdef __cplusplus
}
#endif
#endif