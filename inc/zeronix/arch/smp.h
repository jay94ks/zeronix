#ifndef __INC_ZERONIX_ARCH_SMP_H__
#define __INC_ZERONIX_ARCH_SMP_H__

/**
 * This header provides architecture specific functions.
 */

#include <zeronix/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* execute callback. */
typedef void(* karch_smp_exec_t)();

/**
 * SMP interface.
 * this will be passed to kernel.
 */
typedef struct {
    uint8_t is_enabled;

    /* returns how many CPUs are available. */
    uint8_t (* count)();

    /* get the count of CPUs. */
    uint8_t (* cpuid)();

    /* returns whether the CPU is boot CPU or not. */
    uint8_t (* is_boot)(uint8_t id);

    /* try to lock CPU-specific spinlock. */
    uint8_t (* trylock)(uint8_t id);

    /* lock CPU-specific spinlock. */
    uint8_t (* lock)(uint8_t id);

    /* unlock CPU-specific spinlock. */
    uint8_t (* unlock)(uint8_t id);

    /* execute a callback on the CPU, this can be called only once. */
    uint8_t (* exec)(uint8_t id, karch_smp_exec_t cb);
} karch_smp_t;

#ifdef __cplusplus
}
#endif
#endif