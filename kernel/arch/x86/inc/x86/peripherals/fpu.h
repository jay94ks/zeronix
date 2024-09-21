#ifndef __KERNEL_ARCH_X86_INC_X86_PERIPHERALS_FPU_H__
#define __KERNEL_ARCH_X86_INC_X86_PERIPHERALS_FPU_H__

#include <zeronix/types.h>
#include <zeronix/arch/x86/taskseg.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    KFPUFL_EXISTS   = 0x01,
    KFPUFL_OSFXSR   = 0x02,
    KFPUFL_SSE      = 0x04,
};

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize FPU on the current running CPU.
 */
void karch_fpu_init();
#endif

/**
 * get the FPU flags.
 */
uint8_t karch_fpu_get_flags();

/**
 * returns whether FPU exists or not.
 */
#define karch_fpu_exists()  \
    ((karch_fpu_get_flags() & KFPUFL_EXISTS) != 0)

/**
 * https://wiki.osdev.org/SSE
 * returns whether KFPUFL_OSFXSR is set or not.
 * --> this means that 
 */
#define karch_fpu_osfxsr()  \
    ((karch_fpu_get_flags() & KFPUFL_OSFXSR) != 0)

/**
 * https://wiki.osdev.org/SSE
 * returns whether KFPUFL_SSE is set or not.
 */
#define karch_fpu_sse()  \
    ((karch_fpu_get_flags() & KFPUFL_SSE) != 0)

/**
 * store FPU state from processor. 
 * if FPU not supported, this returns zero.
 */
uint8_t karch_fpu_store(karch_fpu_t* state);

/**
 * restore FPU state into processor.
 * if FPU not supported, this returns zero.
 */
uint8_t karch_fpu_restore(const karch_fpu_t* state);
#ifdef __cplusplus
}
#endif

#endif