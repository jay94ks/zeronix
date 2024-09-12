#ifndef __KERNEL_INCLUDE_ZERONIX_ARCH_X86_CPU_H__
#define __KERNEL_INCLUDE_ZERONIX_ARCH_X86_CPU_H__

#include <zeronix/types.h>
#include <zeronix/arch/x86/types.h>

/**
 * Flags for karch_cpu_t.
 */
enum {
    CPUFLAG_INIT_BSP = 1,           // --> is_bsp flag is usable or not.
    CPUFLAG_INIT_LAPIC = 2,         // --> apic_id is available or not.
    CPUFLAG_INIT_LAPIC_FREQ = 4,    // --> lapic frequency.
    CPUFLAG_INIT_FREQ = 8,          // --> frequency.
    CPUFLAG_INIT_TSS = 16,          // --> tss and stackmark is set or not.
};

/**
 * karch_cpu_t.
 * this should be aligned as 8 bytes.
 * ----
 * karch_cpu_t cpu[MAX_CPU] __aligned(8);
 * apic_id: assigned from `karch_apic_collect` at `arch/x86/smp/apic.c`.
 * tss, stackmark: assigned from `karch_setup_tss` at `arch/x86/min86/tss.c`.
 * lapic_freq, freq: assigned from `karch_lapic_calibrate_clocks` at `arch/x86/smp/apic.c`.
 */
typedef struct {
    uint16_t            flags;          // --> flags
    uint8_t             is_bsp;         // --> indicates whether the CPI is BSP or not.
    uint8_t             apic_id;        // --> apic id.
    uint32_t            freq;           // --> CPU frequency.
    uint32_t            lapic_freq;     // --> LAPIC bus frequency.
    uint32_t            pad;
    karch_tss_t         tss;            // --> task segment.
    karch_stackmark_t*  stackmark;      // --> stack mark.
} karch_cpu_t;

// --------------
// all functions in this file are implemented at `arch.c` file.
// --------------

#ifdef __cplusplus
extern "C" {
#endif

/**
 * emergency print message.
 * this is only usable until kernel initialization process.
 * from `karch_init()` to `karch_init_smp` including `karch_smp_start_ap`).
 */
void karch_emergency_print(const char* msg);

/**
 * get the kernel stack for specified cpu.
 * this can be called at anywhere.
 * this defined at arch/x86/impl/src/arch.c
 * 
 * @param max: see `MAX_CPU` value.
 * if max value over, this returns null.
 */
uint8_t* karch_stacktop_for(uint8_t cpu);

/**
 * get the CPU count.
 */
uint8_t karch_count_cpu();

/**
 * get the CPU information.
 * if no CPU available, this returns null.
 */
karch_cpu_t* karch_get_cpu(uint8_t n);

/**
 * indicates whether the CPU is BSP or not.
 * 1) min86 mode or SMP not supported: n == 0 --> 1.
 * 2) BSP initialized: cpus[n].is_bsp.
 * 3) otherwise: always zero.
 */
uint8_t karch_is_bsp(uint8_t n);

/**
 * indicates whether the current CPU is BSP or not.
 * 1) min86 mode or SMP not supported: always non-zero.
 * 2) result of `karch_is_bsp( karch_smp_cpuid_current() )`.
 */
uint8_t karch_is_bsp_cpu_current();

#ifdef __ARCH_X86_INTERNALS__

/**
 * set the count of CPUs on system.
 * returns non-zero value if succeed.
 */
uint8_t karch_set_count_cpu(uint8_t n);

#endif

#ifdef __cplusplus
}
#endif

#endif