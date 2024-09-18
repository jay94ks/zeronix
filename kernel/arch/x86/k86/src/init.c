#define __ARCH_X86_INTERNALS__
#include <x86/k86.h>
#include <x86/smp.h>
#include <x86/klib.h>
#include <x86/k86/cpulocal.h>
#include <x86/interface.h>

// --
void karch_emergency_print(const char* msg);

// --
void karch_init(bootinfo_t* info) {
    // --> initialize the early x86 environment.
    karch_k86_init(info);

    // --> initialize SMP and if it has fatal error.
    if (karch_smp_init() == 0) {
        cpu_cli();

        // kernel panic: unreachable if SMP successfully initialized.
        karch_emergency_print(
            "fatal: the CPU reached to impossible point in SMP initialization.");

        while(1);
    }

    // --> if reached to here, 
    //   : the SMP init failed due to not supported hardware.
    karch_k86_enter_kmain();
}

void karch_emergency_print(const char* msg) {
    uint16_t* vba = (uint16_t*) 0xb8000;
    const char* msg_ = msg;

    while(*msg_) {
        *vba++ = (*msg_++) | (15 << 8);
    }

    *vba++ = '.' | (15 << 8);
    *vba++ = (15 << 8);
}
