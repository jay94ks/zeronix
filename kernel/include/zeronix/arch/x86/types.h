#ifndef __ZERONIX_ARCH_X86_TYPES_H__
#define __ZERONIX_ARCH_X86_TYPES_H__

#include <zeronix/types.h>
#include "i686.h"

/**
 * karch_x86_desc_t.
 */
struct karch_x86_desc_t {
    uint16_t limit;
    uint32_t base;
} __packed;

/**
 * karch_x86_gate_t.
 */
struct karch_x86_gate_t {
    uint16_t off_low;
    uint16_t sel;
    uint8_t pad;         /* |000|XXXXX| ig & trpg, |XXXXXXXX| task g */
    uint8_t p_dpl_type;  /* |P|DL|0|TYPE| */
    uint16_t off_high;
} __packed;

/**
 * karch_x86_seg_t.
 */
struct karch_x86_seg_t {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;         /* |P|DL|1|X|E|R|A| */
    uint8_t granularity;    /* |G|X|0|A|LIMT| */
    uint8_t base_high;
} __packed;

/**
 * Short-hand reference.
 */
typedef struct karch_x86_desc_t karch_desc_t;
typedef struct karch_x86_gate_t karch_gate_t;
typedef struct karch_x86_seg_t karch_seg_t;

/* meaningful aliases. */
typedef uint32_t karch_atom_t;
typedef uint32_t karch_reg32_t;
typedef uint32_t karch_reg16_t;

/**
 * karch_x86_tss_t.
 * task descriptor.
*/
struct karch_x86_tss_t {
    karch_reg32_t backlink;
    karch_reg32_t sp0;
    karch_reg32_t ss0;
    karch_reg32_t sp1;
    karch_reg32_t ss1;
    karch_reg32_t sp2;
    karch_reg32_t ss2;
    karch_reg32_t cr3;
    karch_reg32_t ip;
    karch_reg32_t flags;
    karch_reg32_t ax;
    karch_reg32_t cx;
    karch_reg32_t dx;
    karch_reg32_t bx;
    karch_reg32_t sp;
    karch_reg32_t bp;
    karch_reg32_t si;
    karch_reg32_t di;
    karch_reg32_t es;
    karch_reg32_t cs;
    karch_reg32_t ss;
    karch_reg32_t ds;
    karch_reg32_t fs;
    karch_reg32_t gs;
    karch_reg32_t ldt;
    karch_reg16_t trap;
    karch_reg16_t iobase;
} __packed;

/**
 * karch_x86_stackmark_t
 */
struct karch_x86_stackmark_t {
    karch_reg32_t proc;
    karch_reg32_t cpu;
} __packed;

/* shorthand. */
typedef struct karch_x86_tss_t karch_tss_t;
typedef struct karch_x86_stackmark_t karch_stackmark_t;

/**
 * karch_x86_intr_frame_t
 * interrupt frame.
 */
struct karch_x86_intr_frame_t {
    /* hardware defined. */
    karch_reg32_t ip;
    karch_reg32_t cs;
    karch_reg32_t flags;

    /* only for crossing priv. */
    karch_reg32_t sp;
    karch_reg32_t ss;
};

/* short hand. */
typedef struct karch_x86_intr_frame_t karch_intr_frame_t;


#endif