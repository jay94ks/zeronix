#ifndef __KERNEL_ARCH_X86_INC_X86_K86_TABLES_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_TABLES_H__

#include <x86/types.h>

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

// -------------

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize GDT and IDT tables.
 */
void karch_tables_init();
#endif

/**
 * descriptors for IDT and GDT.
 */
typedef struct {
    karch_desc_t idt;
    karch_desc_t gdt;
} karch_table_desc_t;

/**
 * get table descriptors.
 */
void karch_tables_get_descriptors(karch_table_desc_t* tables);

/**
 *  get the N'th GDT segment pointer.
 */
karch_seg_t* karch_tables_gdt_ptr(uint8_t n);

/**
 * flush current GDT entries to the CPU.
 */
void karch_tables_flush_gdt();

/**
 * flush current IDT entries to the CPU.
 */
void karch_tables_flush_idt();

/**
 * set the `seg` as data segment.
 */
void karch_tables_gdt_data(karch_seg_t* seg, uint32_t base, uint32_t size, uint32_t priv);

/**
 * clear all interrupt vector to `not-mapped`.
*/
void karch_tables_reset_idt();

/**
 * set N'th interrupt descriptor.
 */
void karch_tables_idt(uint8_t vec, uint32_t offset, uint32_t priv);

/**
 * load IDT gate vectors.
 * vec_base: added on `vec` of each element.
 */
void karch_tables_load_idt(const struct gatevec* table, uint8_t vec_base);
#ifdef __cplusplus
}
#endif


// ------------
#define PRIV_KERN   0
#define PRIV_USER   3
#define PRIV_MASK   0x03

// --
#define MAX_IDT 256
#define MAX_CPU 16

enum {
    GDT_NULL = 0,
    GDT_KERN_CS,
    GDT_KERN_DS,
    GDT_USER_CS,
    GDT_USER_DS,
    GDT_LDT,
    GDT_TSS,

    /* TODO: tss per cpu. */

    GDT_MAX = GDT_TSS + MAX_CPU
};

/* Index for TSS. */
#define GDT_TSS_INDEX(x)    (GDT_TSS + (x))

// --> segment selector.
#define SEG_SEL(x)          ((x) * 8)
#define SEG_TSS(x)          (SEG_SEL(GDT_TSS) + (x))

// --> segment predefines.
enum {
    SEG_KERN_CS = SEG_SEL(GDT_KERN_CS),
    SEG_KERN_DS = SEG_SEL(GDT_KERN_DS),
    SEG_USER_CS = SEG_SEL(GDT_USER_CS) | PRIV_USER,
    SEG_USER_DS = SEG_SEL(GDT_USER_DS) | PRIV_USER
}; 

// --
#define BYTE_GRAN_MAX   0xFFFFFL   /* maximum size for byte granular segment */
#define GRANULARITY_SHIFT   16  /* shift for limit --> granularity */
#define OFFSET_HIGH_SHIFT   16  /* shift for (gate) offset --> offset_high */
#define PAGE_GRAN_SHIFT     12  /* extra shift for page granular limits */

// --
#define GRANULAR  	  0x80	/* set for 4K granularilty */
#define DEFAULT   	  0x40	/* set for 32-bit defaults (executable seg) */
#define BIG       	  0x40	/* set for "BIG" (expand-down seg) */
#define AVL        	  0x10	/* 0 for available */
#define LIMIT_HIGH   	  0x0F	/* mask for high bits of limit */

/* Selector bits. */
#define TI                0x04	/* table indicator */
#define RPL               0x03	/* requester privilege level */

/* Base and limit sizes and shifts. */
#define BASE_MIDDLE_SHIFT   16	/* shift for base --> base_middle */

/* Access-byte and type-byte bits. */
#define PRESENT           0x80	/* set for descriptor present */
#define DPL               0x60	/* descriptor privilege level mask */
#define DPL_SHIFT            5
#define SEGMENT           0x10	/* set for segment-type descriptors */

/* Access-byte bits. */
#define EXECUTABLE        0x08	/* set for executable segment */
#define CONFORMING        0x04	/* set for conforming segment if executable */
#define EXPAND_DOWN       0x04	/* set for expand-down segment if !executable*/
#define READABLE          0x02	/* set for readable segment if executable */
#define WRITEABLE         0x02	/* set for writeable segment if !executable */
#define TSS_BUSY          0x02	/* set if TSS descriptor is busy */
#define ACCESSED          0x01	/* set if segment accessed */

/* Special descriptor types. */
#define AVL_286_TSS          1	/* available 286 TSS */
#define LDT                  2	/* local descriptor table */
#define BUSY_286_TSS         3	/* set transparently to the software */
#define CALL_286_GATE        4	/* not used */
#define TASK_GATE            5	/* only used by debugger */
#define INT_286_GATE         6	/* interrupt gate, used for all vectors */
#define TRAP_286_GATE        7	/* not used */

#define INT_GATE_TYPE	(INT_286_GATE | DESC_386_BIT)
#define TSS_TYPE	    (AVL_286_TSS  | DESC_386_BIT)

/* Descriptor structure offsets. */
#define DESC_GRANULARITY     6	/* to granularity byte */
#define DESC_BASE_HIGH       7	/* to base_high */

/* Type-byte bits. */
#define DESC_386_BIT  0x08 /* 386 types are obtained by ORing with this */
				/* LDT's and TASK_GATE's don't need it */

#endif