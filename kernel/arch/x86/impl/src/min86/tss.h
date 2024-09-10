#ifndef __ZERONIX_ARCH_X86_LEGACY_TSS_H__
#define __ZERONIX_ARCH_X86_LEGACY_TSS_H__

#include <zeronix/types.h>
#include <zeronix/arch/x86/types.h>

/**
 * initialize TSS.
 */
void karch_init_tss();

/**
 * get the TSS at N'th.
 */
karch_tss_t* karch_get_tss(uint8_t n);

/**
 * setup TSS for N'th.
 */
uint8_t karch_setup_tss(uint8_t cpu, void* stack_top);

#endif