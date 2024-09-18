#ifndef __KERNEL_ARCH_X86_INC_X86_K86_CPULOCAL_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_CPULOCAL_H__

#include <zeronix/types.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Maximum count for CPU local variables.
 */
#define MAX_CPU_LOCALS      8

/**
 * CPU local variables.
 */
struct karch_x86_cpuvar_t {
    union {
        void*    ptr;
        uint64_t u64;
        uint32_t u32;
        uint16_t u16;
        uint8_t  u8;
        int64_t  s64;
        int32_t  s32;
        int16_t  s16;
        int8_t   s8;
    };
} __packed;

typedef struct karch_x86_cpuvar_t karch_cpuvar_t;

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize CPU local variables. 
 * this is called after SMP initialization completed.
 * and if SMP not supported, this'll be called after `karch_smp_init` call.
 */
void karch_cpulocals_init();
#endif

/**
 * get the cpu local variable at specified slot.
 * if no slot available, this returns zero.
 */
uint8_t karch_cpulocals_get(uint8_t slot, karch_cpuvar_t* var);

/**
 * get the cpu local variable at specified slot.
 * if no slot available, this returns zero.
 */
uint8_t karch_cpulocals_get_n(uint8_t cpu, uint8_t slot, karch_cpuvar_t* var);

/**
 * set the cpu local variable at specified slot.
 * if no slot available, this returns zero.
 */
uint8_t karch_cpulocals_set(uint8_t slot, const karch_cpuvar_t* var);

/**
 * set the cpu local variable at specified slot.
 * if no slot available, this returns zero.
 */
uint8_t karch_cpulocals_set_n(uint8_t cpu, uint8_t slot, const karch_cpuvar_t* var);

/**
 * short-form for getting pointer using karch_cpulocals_get.
 * if no slot available, this returns null.
 */
void* karch_cpulocals_get_ptr(uint8_t slot);

/**
 * short-form for getting pointer using karch_cpulocals_get.
 * if no slot available, this returns zero.
 */
uint8_t karch_cpulocals_set_ptr(uint8_t slot, void* ptr);

#ifdef __cplusplus
}
#endif
#endif