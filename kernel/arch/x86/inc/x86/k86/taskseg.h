#ifndef __KERNEL_ARCH_X86_INC_X86_K86_TASKSEG_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_TASKSEG_H__

#include <zeronix/arch/x86/taskseg.h>
#include <x86/types.h>

#ifdef __cplusplus
extern "C" {
#endif

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