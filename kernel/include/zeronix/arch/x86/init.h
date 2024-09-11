#ifndef __KERNEL_INCLUDE_ZERONIX_ARCH_X86_ARCH_H__
#define __KERNEL_INCLUDE_ZERONIX_ARCH_X86_ARCH_H__

#include <zeronix/types.h>

/**
 * get the kernel stack for specified cpu.
 * this can be called at anywhere.
 * this defined at arch/x86/impl/src/arch.c
 * 
 * @param max: see `MAX_CPU` value.
 * if max value over, this returns null.
 */
uint8_t* karch_stacktop_for(uint8_t cpu);

#endif