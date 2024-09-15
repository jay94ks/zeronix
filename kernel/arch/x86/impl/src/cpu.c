#define __ARCH_X86_INTERNALS__

#include <zeronix/types.h>
#include <zeronix/boot.h>
#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/layout.h>
#include <zeronix/arch/x86/cpu.h>

// --
extern void* _karch_tss0_stack;
extern uint8_t mode_min86;  // --> defined at arch.c

// --
karch_cpu_t cpus[MAX_CPU] __aligned(8);
uint8_t cpu_n;

void karch_init_cpu() {
    kmemset(cpus, 0, sizeof(cpus));
    
    cpu_n = 1;
}

uint8_t* karch_stacktop_for(uint8_t cpu) {
    if (cpu >= MAX_CPU) {
        return 0;
    }

    uint8_t* top = (uint8_t*) &_karch_tss0_stack;
    return top - (I686_PAGE_SIZE * cpu);
}

uint8_t karch_count_cpu() {
    return cpu_n;
}

karch_cpu_t* karch_get_cpu(uint8_t n) {
    if (n >= cpu_n) {
        return 0;
    }

    return &cpus[n];
}

uint8_t karch_set_count_cpu(uint8_t n) {
    if (n >= MAX_CPU) {
        return 0;
    }

    cpu_n = n;
    return 1;
}

uint8_t karch_is_bsp_cpu(uint8_t n) {
    if (n >= MAX_CPU || n >= cpu_n) {
        return 0;
    }

    if (mode_min86 || !karch_smp_supported()) {
        return n == 0 ? 1 : 0;
    }

    if ((cpus[n].flags & CPUFLAG_INIT_BSP) != 0) {
        return cpus[n].is_bsp;
    }

    return 0;
}

uint8_t karch_is_bsp_cpu_current() {
    if (mode_min86 || !karch_smp_supported()) {
        return 1;
    }

    uint8_t smp_cpu = karch_smp_cpuid_current();
    return karch_is_bsp_cpu(smp_cpu);
}

karch_cpu_t* karch_get_cpu_current() {
    if (mode_min86 || !karch_smp_supported()) {
        return &cpus[0];
    }

    return karch_get_cpu(karch_smp_cpuid_current());
}

uint8_t karch_set_smp_mode(uint8_t cpu_id, karch_cpu_smpmode_t mode) {
    karch_cpu_t* cpu = karch_get_cpu(cpu_id);
    if (cpu) {
        cpu->smp_mode = mode;
        return 1;
    }

    return 0;
}

uint8_t karch_set_cpu_init_flags(uint8_t cpu_id, uint32_t flag) {
    karch_cpu_t* cpu = karch_get_cpu(cpu_id);
    if (cpu) {
        cpu->flags |= flag;
        return 1;
    }

    return 0;
}

uint8_t karch_unset_cpu_init_flags(uint8_t cpu_id, uint32_t flag) {
    karch_cpu_t* cpu = karch_get_cpu(cpu_id);
    if (cpu) {
        cpu->flags &= ~flag;
        return 1;
    }

    return 0;
}
