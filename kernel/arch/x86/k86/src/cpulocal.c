#include <x86/k86/cpulocal.h>
#include <x86/k86/tables.h>
#include <x86/smp.h>

#include <zeronix/kstring.h>

// --
typedef struct {
    karch_cpuvar_t vars[MAX_CPU_LOCALS];
} cpulocal_t;

// --
cpulocal_t cpulocals[MAX_CPU] __aligned(8);

// --
void karch_cpulocals_init() {
    int32_t cpu = karch_smp_cpuid();
    if (cpu < 0) {
        kmemset(&cpulocals[0], 0, sizeof(cpulocal_t));
        return;
    }

    kmemset(&cpulocals[cpu], 0, sizeof(cpulocal_t));
}

uint8_t karch_cpulocals_get(uint8_t slot, karch_cpuvar_t* var) {
    if (slot >= MAX_CPU_LOCALS || !var) {
        return 0;
    }

    int32_t cpu = karch_smp_cpuid();
    if (cpu < 0) {
        cpu = 0;
    }

    kmemcpy(var, &cpulocals[cpu].vars[slot], sizeof(karch_cpuvar_t));
    return 1;
}

uint8_t karch_cpulocals_get_n(uint8_t cpu, uint8_t slot, karch_cpuvar_t* var) {
    if (cpu > MAX_CPU || slot >= MAX_CPU_LOCALS || !var) {
        return 0;
    }

    kmemcpy(var, &cpulocals[cpu].vars[slot], sizeof(karch_cpuvar_t));
    return 1;
}

uint8_t karch_cpulocals_set(uint8_t slot, const karch_cpuvar_t* var) {
    if (slot >= MAX_CPU_LOCALS || !var) {
        return 0;
    }

    int32_t cpu = karch_smp_cpuid();
    if (cpu < 0) {
        cpu = 0;
    }

    kmemcpy(&cpulocals[cpu].vars[slot], var, sizeof(karch_cpuvar_t));
    return 1;
}

uint8_t karch_cpulocals_set_n(uint8_t cpu, uint8_t slot, const karch_cpuvar_t* var) {
    if (cpu > MAX_CPU || slot >= MAX_CPU_LOCALS || !var) {
        return 0;
    }

    kmemcpy(&cpulocals[cpu].vars[slot], var, sizeof(karch_cpuvar_t));
    return 1;
}

void* karch_cpulocals_get_ptr(uint8_t slot) {
    karch_cpuvar_t var;

    if (karch_cpulocals_get(slot, &var)) {
        return var.ptr;
    }

    return 0;
}

uint8_t karch_cpulocals_set_ptr(uint8_t slot, void* ptr) {
    karch_cpuvar_t var = {{ptr}};
    //var.ptr = ptr;

    return karch_cpulocals_set(slot, &var);
}