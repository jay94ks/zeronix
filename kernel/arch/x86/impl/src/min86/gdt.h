#ifndef __ZERONIX_ARCH_X86_LEGACY_GDT_H__
#define __ZERONIX_ARCH_X86_LEGACY_GDT_H__

#include <zeronix/types.h>
#include <zeronix/arch/x86/types.h>

/**
 * initialize GDT entries for system environment. 
 */
void karch_init_gdt();

/**
 * flush current GDT entries to the CPU.
 */
void karch_flush_gdt();

/**
 * get the N'th GDT entry.
 */
karch_seg_t* karch_get_gdt(uint8_t n);

/**
 * set the `seg` as data segment.
 */
void karch_set_gdt_data(karch_seg_t* seg, uint32_t base, uint32_t size, uint32_t priv);

#endif