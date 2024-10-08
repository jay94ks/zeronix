#ifndef __KERNEL_ARCH_X86_INC_X86_K86_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_H__

#include <x86/types.h>
#ifdef __ARCH_X86_INTERNALS__
#include <x86/boot/bootinfo.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_X86_INTERNALS__
/**
 * initalize the minimal K86 module.
 * this is for initializing the early x86 environment.
 */
void karch_k86_init(bootinfo_t* info);

/**
 * enter to `kmain`. this must be called for BSP CPU.
 */
void karch_k86_enter_kmain();
#endif

#ifdef __cplusplus
}
#endif
#endif