#ifndef __KERNEL_ARCH_X86_INC_X86_K86_MMAP_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_MMAP_H__

#include <zeronix/types.h>
#ifdef __ARCH_X86_INTERNALS__
#include <x86/boot/bootinfo.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize the memory map for architecture.
 * this only keeps free-regions.
 */
void karch_mmap_init(bootinfo_t* info);
#endif


#ifdef __cplusplus
}
#endif

#endif