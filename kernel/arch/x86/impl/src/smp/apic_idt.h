// kernel_arch_x86_impl_src_smp_apic_idt_h
// PRESS TAB TO GENERATE COMPLETELY.

#ifndef __KERNEL_ARCH_X86_IMPL_SRC_SMP_APIC_IDT_H__
#define __KERNEL_ARCH_X86_IMPL_SRC_SMP_APIC_IDT_H__

#include <zeronix/types.h>

/**
 * initialize APIC's IDT.
 */
void karch_init_apic_idt();

/**
 * de-initialize APIC's IDT.
 * this restores min86's PIC interrupt vectors.
 */
void karch_deinit_apic_idt();

#endif