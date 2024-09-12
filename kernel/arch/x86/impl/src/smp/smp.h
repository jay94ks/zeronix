// kernel_arch_x86_impl_src_smp_smp_h
// PRESS TAB TO GENERATE COMPLETELY.

#ifndef __KERNEL_ARCH_X86_IMPL_SRC_SMP_SMP_H__
#define __KERNEL_ARCH_X86_IMPL_SRC_SMP_SMP_H__

#include <zeronix/types.h>
#define SMP_CPUID_LAPIC_AUTO      0xff

/**
 * initialize SMP and prepare environment. 
 */
uint8_t karch_init_smp();

/**
 * returns whether SMP is supported and prepared or not.
 */
uint8_t karch_smp_supported();

/**
 * returns the CPU ID by APIC ID. 
 */
uint8_t karch_smp_cpuid(uint8_t lapic_id);

/**
 * returns the CPU ID by current running APIC.
 */
uint8_t karch_smp_cpuid_current();
#endif