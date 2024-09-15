#ifndef __KERNEL_ARCH_X86_INC_X86_SMP_H__
#define __KERNEL_ARCH_X86_INC_X86_SMP_H__
#include <x86/types.h>

#include <x86/spinlock.h>
#include <x86/k86/taskseg.h>
#include <x86/peripherals/apic.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * returns whether SMP is supported and prepared or not.
 */
uint8_t karch_smp_supported();

#ifdef __ARCH_X86_INTERNALS__

/**
 * initalize the minimal SMP module.
 * this returns:
 * 1) ret < 0: not supported.
 * 2) ret = 0: failed, fatal error.
 * 3) otherwise, this will not return to caller.  
 */
int32_t karch_smp_init();
#endif

/**
 * SMP CPU information.
 */
typedef struct {
    uint8_t                 ready;
    uint8_t                 cpu_id;     // --> CPU number, aka, LAPIC number.
    karch_tss_t*            tss;        // --> TSS for the CPU.
    karch_spinlock_t*       lock;       // --> lock for specific cpu.
    karch_stackmark_t*      stackmark;  // --> stack mark.
    const karch_lapic_t*    lapic;      // --> lapic info.
    const karch_cpuinfo_t*  ident;      // --> CPU identification.
} karch_smp_cpuinfo_t;

/**
 * get the CPU information.
 * returns non zero value if succeed.
 * this can even works without SMP also but, APIC is required.
 */
uint8_t karch_smp_get_cpuinfo(uint8_t n, karch_smp_cpuinfo_t* info);

/**
 * get the current running CPU information.
 * returns non zero value if succeed.
 * this can even works without SMP also but, APIC is required.
 */
uint8_t karch_smp_get_cpuinfo_current(karch_smp_cpuinfo_t* info);
#ifdef __cplusplus
}
#endif

#endif