#ifndef __ZERONIX_ARCH_X86_CPUINFO_H__
#define __ZERONIX_ARCH_X86_CPUINFO_H__

#include <zeronix/types.h>

/**
 * kcpu_vendor_t.
 */
typedef enum {
    KCPU_INTEL = 0,
    KCPU_AMD
} karch_vendor_t;

/**
 * kcpu_feature_t.
*/
typedef enum {
    KCPUF_PSE = 1,
    KCPUF_PAE,
    KCPUF_PGE,
    KCPUF_APIC_ON_CHIP,
    KCPUF_TSC,
    KCPUF_FPU,
    KCPUF_SSE_FULL,
    KCPUF_FXSR,
    KCPUF_SSE1,
    KCPUF_SSE2,
    KCPUF_SSE3,
    KCPUF_SSSE3,
    KCPUF_SSE4_1,
    KCPUF_SSE4_2,
    KCPUF_HTT,
    KCPUF_HTT_MAX,
    KCPUF_SYSENTER,
    KCPUF_SYSCALL
} karch_feature_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get cpu feature flag and vendor info.
 * this is safe except unpaged region or user space.
 */
uint32_t kcpuinfo(karch_feature_t feature);

#ifdef __cplusplus
}
#endif

#endif