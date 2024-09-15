#ifndef __KERNEL_ARCH_X86_INC_X86_ENV_H__
#define __KERNEL_ARCH_X86_INC_X86_ENV_H__
#include <x86/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CPU informations that collected by `cpuid` instruction.
 */
typedef struct {
    uint32_t vendor[4];
    uint32_t feature[4];
} karch_cpuinfo_t;

/**
 * field indices for `karch_cpuinfo_t`.
 */
enum {
    CPUINFO_EAX = 0,
    CPUINFO_EBX,
    CPUINFO_ECX,
    CPUINFO_EDX
};

/**
 * get the CPU informations.
 * returns non-zero value if succeed.
 */
uint8_t karch_env_cpuinfo(karch_cpuinfo_t* out);

/**
 *  System call type.
 */
enum {
    ENV_USE_SYSENTER    = 1,    // 01
    ENV_USE_SYSCALL     = 2,    // 10
    ENV_USE_BOTH        = 3,    // 11
};

/**
 * get the syscall type.
 * this returns ENV_USE_SYSENTER, ENV_USE_SYSCALL or both.
 */
uint8_t karch_env_syscall_type();

#ifdef __cplusplus
}
#endif

#endif