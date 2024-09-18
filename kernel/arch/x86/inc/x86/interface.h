#ifndef __KERNEL_ARCH_X86_INC_X86_INTERFACE_H__
#define __KERNEL_ARCH_X86_INC_X86_INTERFACE_H__

#include <zeronix/arch/smp.h>

/**
 * make an interface to access SMP feature.
 */
uint8_t karch_interface_smp(karch_smp_t* smp);
#endif