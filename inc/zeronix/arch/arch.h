#ifndef __INC_ZERONIX_ARCH_ARCH_H__
#define __INC_ZERONIX_ARCH_ARCH_H__

#include <zeronix/types.h>
#include <zeronix/arch/smp.h>
#include <zeronix/arch/mmap.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * callback type for SYSTICK handler.
 */
typedef void(* karch_systick_t)();

/**
 * architecture specific function interface.
 */
typedef struct {
    const karch_mmap_t* mem_free;
    const karch_mmap_t* mem_kern_virt;

    /* SMP interface. */
    karch_smp_t smp;
    
    /* SYSTICK frequency in hz. */
    uint32_t systick_freq;
} karch_t;

/**
 * get the kernel architecture specific layer interface.
 */
void karch_interface(karch_t* arch);

#ifdef __cplusplus
}
#endif
#endif