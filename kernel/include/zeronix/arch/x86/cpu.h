#ifndef __KERNEL_INCLUDE_ZERONIX_ARCH_X86_CPU_H__
#define __KERNEL_INCLUDE_ZERONIX_ARCH_X86_CPU_H__

#include <zeronix/types.h>
#include <zeronix/arch/x86/types.h>

/* CPU local variable type. */
typedef struct { uint8_t _; } *karch_cpuvar_t;

/**
 * Flags for karch_cpu_t.
 */
enum {
    CPUFLAG_INIT_BSP        = 0x0001, // --> is_bsp flag is usable or not.
    CPUFLAG_INIT_LAPIC      = 0x0002, // --> apic_id is available or not.
    CPUFLAG_INIT_LAPIC_FREQ = 0x0004, // --> lapic frequency.
    CPUFLAG_INIT_FREQ       = 0x0008, // --> frequency.
    CPUFLAG_INIT_TSS        = 0x0010, // --> tss and stackmark is set or not.
    CPUFLAG_INIT_SMP_MODE   = 0x0020, // --> smp_mode is available or not.
    CPUFLAG_INIT_SMP_BOOT   = 0x0040, // --> the CPU is started and ready.
    CPUFLAG_INIT_IDENT      = 0x0080, // --> CPU identification.
};

/**
 * Modes for `smp_mode`.
 */
typedef enum {
    CPUSMPF_NOT_SMP = 0,
    CPUSMPF_BSP,                    // --> the CPU initialized as BSP.
    CPUSMPF_NOT_BSP                 // --> the CPU is not BSP.
} karch_cpu_smpmode_t;

enum {
    CPUVARS_ZERO = 0,

    __CPUVARS_MAX,
    CPUVARS_MAX = __CPUVARS_MAX
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
    karch_tss_t         tss;                // --> task segment.
    karch_stackmark_t*  stackmark;          // --> stack mark.
    karch_cpuvar_t      vars[CPUVARS_MAX];  // --> CPU local variables.

    uint16_t            flags;              // --> flags: CPUFLAG_INIT_*.
    uint8_t             is_bsp;             // --> indicates whether the CPI is BSP or not.
    uint8_t             apic_id;            // --> apic id.
    uint32_t            freq;               // --> CPU frequency.
    uint32_t            lapic_freq;         // --> LAPIC bus frequency.
    uint8_t             smp_mode;           // --> SMP mode: one of CPUSMPF_*.
    uint8_t             pad;

    /* identification values: filled by SMP.c */
    uint8_t             ident_step;
    uint8_t             ident_family;
    uint8_t             ident_model;
    uint32_t            ident_ecx;
    uint32_t            ident_edx;
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

/**
 * get the current CPU struct.
 * 1) min86 mode or SMP not supported: karch_get_cpu(0).
 * 2) result of `karch_get_cpu( karch_smp_cpuid_current() )`
 */
karch_cpu_t* karch_get_cpu_current();

#ifdef __ARCH_X86_INTERNALS__

/**
 * set the count of CPUs on system.
 * returns non-zero value if succeed.
 */
uint8_t karch_set_count_cpu(uint8_t n);

/**
 * set the CPU's SMP mode value.
*/
uint8_t karch_set_smp_mode(uint8_t cpu_id, karch_cpu_smpmode_t mode);

/**
 * set the CPU flags for specified cpu id.
 * returns non-zero value if succeed.
*/
uint8_t karch_set_cpu_init_flags(uint8_t cpu_id, uint32_t flag);

/**
 * unset the CPU flags for specified cpu id.
 * returns non-zero value if succeed.
*/
uint8_t karch_unset_cpu_init_flags(uint8_t cpu_id, uint32_t flag);

#endif

#ifdef __cplusplus
}
#endif

#endif