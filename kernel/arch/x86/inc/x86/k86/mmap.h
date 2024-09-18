#ifndef __KERNEL_ARCH_X86_INC_X86_K86_MMAP_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_MMAP_H__

#include <zeronix/types.h>
#include <zeronix/arch/mmap.h>
#ifdef __ARCH_X86_INTERNALS__
#include <x86/boot/bootinfo.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_X86_INTERNALS__
#ifndef __NO_MMAP_EXTERNS__
extern karch_mmap_t mmap_free;          // --> free, avail phys.
extern karch_mmap_t mmap_kern_virt;     // --> reserved, unavail virts, kernel space.
#endif

/**
 * initialize the memory map for architecture.
 * this only keeps free-regions.
 */
void karch_mmap_init(bootinfo_t* info);
#endif

/**
 * add a memory region to memory map.
 */
uint8_t karch_mmap_add(karch_mmap_t* mmap, uint32_t addr, uint32_t len);

/**
 * subtract memort region from memory map.
 */
uint8_t karch_mmap_subtract(karch_mmap_t* mmap, uint32_t addr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif