#ifndef __ZERONIX_ARCH_LEGACY_i8253_H__
#define __ZERONIX_ARCH_LEGACY_i8253_H__

#include <zeronix/types.h>

/**
 * initialize the i8253 timer.
 */
void karch_init_i8253(uint32_t freq);

/**
 * de-initialize the i8253 timer.
 */
void karch_deinit_i8253();
#endif