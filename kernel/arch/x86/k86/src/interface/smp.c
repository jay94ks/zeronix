#include <x86/smp.h>
#include <x86/interface.h>
#include <x86/k86/cpulocal.h>
#include <x86/peripherals/apic.h>

#include <zeronix/arch/smp.h>
#include <zeronix/kstring.h>

// --
uint8_t smp_is_boot(uint8_t id);
uint8_t smp_emit_ipi(uint8_t id, uint8_t irq);

uint8_t smp_cpuid() {
    return karch_smp_cpuid();
}

// --
uint8_t karch_interface_smp(karch_smp_t* smp) {
    if (!smp || !karch_smp_supported()) {
        return 0;
    }

    kmemset(smp, 0, sizeof(karch_smp_t));

    if (karch_smp_supported()) {
        smp->is_enabled = 1;
        smp->is_boot = smp_is_boot;
        smp->count = karch_smp_cpus;
        smp->cpuid = smp_cpuid;
        smp->trylock = karch_smp_trylock;
        smp->lock = karch_smp_lock;
        smp->unlock = karch_smp_unlock;
        smp->exec = karch_smp_jump;
        smp->emit_ipi = smp_emit_ipi;
    }
}

uint8_t smp_is_boot(uint8_t id) {
    return id == karch_smp_bspid() ? 1 : 0;
}

uint8_t smp_emit_ipi(uint8_t id, uint8_t irq) {
    if (karch_smp_cpuid() == id) {
        return karch_lapic_emit_ipi(irq, id, LAPICIPI_SELF);
    }
    
    return karch_lapic_emit_ipi(irq, id, LAPICIPI_SPECIFIC);
}