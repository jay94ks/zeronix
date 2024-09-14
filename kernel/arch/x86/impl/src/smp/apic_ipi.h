#ifndef __KERNEL_ARCH_X86_IMPL_SRC_SMP_APIC_IPL_H__
#define __KERNEL_ARCH_X86_IMPL_SRC_SMP_APIC_IPL_H__

#include <zeronix/types.h>

/**
 * send init ipl to specified cpu. 
 * returns non-zero value if succeed.
 */
uint8_t karch_lapic_send_init_ipi(uint8_t cpu_id);

/**
 * send startup ipi to specified cpu.
 * returns non-zero value if succeed.
 */
uint8_t karch_lapic_send_startup_ipi(uint8_t cpu_id, uint32_t entry_point);

#endif