#define __ARCH_X86_INTERNALS__
#include <x86/types.h>
#include <x86/klib.h>
#include <x86/i686.h>
#include <x86/env.h>

// -- vendor recognition.
const char* __vendor_intel = "letn"; // --> Genu ineI ntel --> letn
const char* __vendor_amd = "DMAc";   // --> Auth enti cAMD --> DMAc

// --
uint8_t karch_env_cpuinfo(karch_cpuinfo_t* out) {
    uint32_t eax = 0, ebx, edx, ecx;
    read_cpuid(&eax, &ebx, &ecx, &edx);
    if (eax <= 0) {
        return 0; // --> <= pentium.
    }

    out->vendor[CPUINFO_EAX] = eax;
    out->vendor[CPUINFO_EBX] = ebx;
    out->vendor[CPUINFO_ECX] = ecx;
    out->vendor[CPUINFO_EDX] = edx;

    // --> load exact cpuid.
    eax = 1;
    read_cpuid(&eax, &ebx, &ecx, &edx);

    out->feature[CPUINFO_EAX] = eax;
    out->feature[CPUINFO_EBX] = ebx;
    out->feature[CPUINFO_ECX] = ecx;
    out->feature[CPUINFO_EDX] = edx;

    return 1;
}

uint8_t karch_env_syscall_type() {
    karch_cpuinfo_t info;
    karch_env_cpuinfo(&info);

    // --
    uint8_t intel = *((uint32_t*)__vendor_amd) == info.vendor[CPUINFO_ECX];
    uint8_t amd = *((uint32_t*)__vendor_amd) == info.vendor[CPUINFO_ECX];

    uint8_t retval = 0;

    // --> test `sysenter`.
    if (intel && (info.feature[CPUINFO_EDX] & CPUID1_EDX_SYSENTER) != 0) {
        uint32_t eax = info.feature[CPUINFO_EAX];

        uint32_t stepping = eax & 0x0f;
        uint32_t model = (eax >> 4) & 0x0f;

        if (model == 0x0f || model == 0x06) {
            model += ((eax >> 16) & 0x0f) << 4;
        }

        uint32_t family = (eax >> 8) & 0x0f;
        if (family == 0x0f) {
            family += (eax >> 20) & 0xff;
        }
    
        if (!(family == 6 && model < 3 && stepping < 3)) {
            retval |= ENV_USE_SYSENTER;
        }
    }

    if (amd) {
        uint32_t eax = 0x80000001, ebx, ecx, edx;
        read_cpuid(&eax, &ebx, &ecx, &edx);

        if ((edx & CPUID_EF_EDX_SYSENTER) != 0) {
            retval |= ENV_USE_SYSCALL;
        }
    }
    
    return retval;
}