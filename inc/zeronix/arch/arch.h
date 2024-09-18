#ifndef __INC_ZERONIX_ARCH_ARCH_H__
#define __INC_ZERONIX_ARCH_ARCH_H__

#include <zeronix/types.h>
#include <zeronix/arch/smp.h>
#include <zeronix/arch/mmap.h>
#include <zeronix/arch/timer.h>

/**
 * architecture specific function interface.
 */
typedef struct {
    const karch_mmap_t* mem_free;
    const karch_mmap_t* mem_kern_virt;

    karch_timer_t timer;
    karch_smp_t smp;

    
} karch_t;


#endif