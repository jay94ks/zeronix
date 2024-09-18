#ifndef __KERNEL_ARCH_X86_INC_X86_SMP_H__
#define __KERNEL_ARCH_X86_INC_X86_SMP_H__
#include <x86/types.h>
#include <x86/env.h>
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

/**
 * returns how many CPUs are available as CPUs.
 */
uint8_t karch_smp_cpus();

/**
 * returns which CPU is boot cpu or not.
 */
uint8_t karch_smp_bspid();

#ifdef __ARCH_X86_INTERNALS__

/**
 * callback type to enter `kmain`. 
 */
typedef void(* karch_enter_kmain_t)();

/**
 * initalize the minimal SMP module.
 * this returns:
 * 1) ret < 0: not supported.
 * 2) ret = 0: failed, fatal error.
 * 3) otherwise, this will not return to caller.  
 */
int32_t karch_smp_init(karch_enter_kmain_t kmain);
#endif

/**
 * get the current running CPU id.
 * cpu id: lapic number.
 * 
 * if SMP is not supported, returns negative value.
 */
int32_t karch_smp_cpuid();

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

/**
 * try to lock the N'th CPU.
 * returns zero if no such CPU available or couldn't lock.
 */
uint8_t karch_smp_trylock(uint8_t n);

/**
 * lock the N'th CPU.
 * returns zero if no such CPU available.
 */
uint8_t karch_smp_lock(uint8_t n);

/**
 * unlock the N'th CPU.
 * returns zero if no such CPU available.
 */
uint8_t karch_smp_unlock(uint8_t n);

/**
 * make the N'th CPU to jump to specified `cb`.
 * this can be called only once.
 * returns non zero value if succeed.
 */
uint8_t karch_smp_jump(uint8_t n, void(* cb)());
#ifdef __cplusplus
}
#endif

#endif
