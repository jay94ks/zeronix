#ifndef __KERNEL_ARCH_X86_INC_X86_K86_TASKSEG_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_TASKSEG_H__

#include <x86/types.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize TSS and setup initial tss for bsp cpu.
 */
void karch_taskseg_init();
#endif

/**
 * get the N'th TSS pointer.
 */
karch_tss_t* karch_taskseg_get(uint8_t n);

/**
 * get the N'th default stack top.
*/
void* karch_taskseg_get_stacktop(uint8_t n);

/**
 * get the N'th stack mark.
 */
karch_stackmark_t* karch_taskseg_get_stackmark(uint8_t n);

/**
 * setup the N'th TSS.
 * if null specified for `stack_top`, 
 * N'th default stack top will be used.
 * (by karch_tss_get_stacktop)
 */
uint8_t karch_taskseg_setup(uint8_t n, void* stack_top);

#ifdef __cplusplus
}
#endif
#endif