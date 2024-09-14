#ifndef __ZERONIX_ARCH_X86_LEGACY_IDT_H__
#define __ZERONIX_ARCH_X86_LEGACY_IDT_H__

#include <zeronix/types.h>
#include <zeronix/arch/x86/types.h>

/**
 * entity for vector table.
*/
struct gatevec {
    void(*gate)(void);
    uint8_t vec;
    uint8_t priv;
};

// --> null vector for `gatevec` type.
#define GATEVEC_NULL    { 0, 0, 0 }

#define GATEVEC_PIC_MASTER  0x50
#define GATEVEC_PIC_SLAVE   0x70

/**
 * initialize the interrupt descriptor table.
 */
void karch_init_idt();

/**
 * (SMP only) get IDT pointer. 
 */
karch_desc_t* karch_get_idt_ptr();

/**
 * clear all interrupt vector to `not-mapped`.
*/
void karch_reset_idt();

/**
 * flush current IDT entries to the CPU.
 */
void karch_flush_idt();

/**
 * set N'th interrupt descriptor.
 */
void karch_set_idt(uint8_t vec, uint32_t offset, uint32_t priv);

/**
 * load IDT gate vectors.
 * vec_base: added on `vec` of each element.
 */
void karch_load_idt_gatevec(const struct gatevec* table, uint8_t vec_base);

#endif