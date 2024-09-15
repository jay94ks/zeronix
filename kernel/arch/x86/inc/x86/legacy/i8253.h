#ifndef __KERNEL_ARCH_X86_INC_X86_LEGACY_I8253_H__
#define __KERNEL_ARCH_X86_INC_X86_LEGACY_I8253_H__

#include <zeronix/types.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * initialize the i8253 timer.
 */
void karch_i8253_init(uint32_t freq);

/**
 * de-initialize the i8253 timer.
 */
void karch_i8253_deinit();

#ifdef __cplusplus
}
#endif
#endif