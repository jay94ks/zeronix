#include <zeronix/arch/x86/cpuinfo.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/i686.h>

// --> Genu ineI ntel --> letn
const char* __vendor_intel = "letn";

// --> Auth enti cAMD --> DMAc
const char* __vendor_amd = "DMAc";

uint32_t kcpuinfo(karch_feature_t feature) {
    if (!feature) {
        return 0;
    }

    uint32_t eax = 0, ebx, edx, ecx;
    uint32_t ef_eax = 0, ef_ebx = 0, ef_edx = 0, ef_ecx = 0;

    read_cpuid(&eax, &ebx, &ecx, &edx);

    if (eax <= 0) {
        return 0; // --> <= pentium.
    }

	const uint32_t v_intel = *((uint32_t*)__vendor_intel);
	const uint32_t v_amd = *((uint32_t*)__vendor_amd);

    uint8_t intel = *((uint32_t*)__vendor_intel) == ecx;
    uint8_t amd = *((uint32_t*)__vendor_amd) == ecx;
    
    // --> load exact cpuid.
    eax = 1;
    read_cpuid(&eax, &ebx, &ecx, &edx);

    uint32_t stepping = eax & 0x0f;
    uint32_t model = (eax >> 4) & 0x0f;

    if (model == 0x0f || model == 0x06) {
        model += ((eax >> 16) & 0x0f) << 4;
    }

    uint32_t family = (eax >> 8) & 0x0f;
    if (family == 0x0f) {
        family += (eax >> 20) & 0xff;
    }

    if (amd) {
        ef_eax = 0x80000001;
        read_cpuid(&ef_eax, &ef_ebx, &ef_ecx, &ef_edx);
    }

    switch (feature) {
        case KCPUF_PSE:
            return edx & CPUID1_EDX_PSE;

        case KCPUF_PAE:
            return edx & CPUID1_EDX_PAE;
            
		case KCPUF_APIC_ON_CHIP:
			return edx & CPUID1_EDX_APIC_ON_CHIP;

		case KCPUF_TSC:
			return edx & CPUID1_EDX_TSC;
            
		case KCPUF_FPU:
			return edx & CPUID1_EDX_FPU;

		case KCPUF_SSE_FULL:
			return	(edx & CPUID1_EDX_SSE_FULL) == CPUID1_EDX_SSE_FULL &&
				    (ecx & CPUID1_ECX_SSE_FULL) == CPUID1_ECX_SSE_FULL;

		case KCPUF_FXSR:
			return edx & CPUID1_EDX_FXSR;

		case KCPUF_SSE1:
			return edx & CPUID1_EDX_SSE;

		case KCPUF_SSE2:
			return edx & CPUID1_EDX_SSE2;

		case KCPUF_SSE3:
			return ecx & CPUID1_ECX_SSE3;

		case KCPUF_SSSE3:
			return ecx & CPUID1_ECX_SSSE3;

		case KCPUF_SSE4_1:
			return ecx & CPUID1_ECX_SSE4_1;

		case KCPUF_SSE4_2:
			return ecx & CPUID1_ECX_SSE4_2;

		case KCPUF_HTT:
			return edx & CPUID1_EDX_HTT;

		case KCPUF_HTT_MAX:
			return (ebx >> 16) & 0xff;

		case KCPUF_SYSENTER:
			if(!intel) return 0;
			if(!(edx & CPUID1_EDX_SYSENTER)) return 0;
			if(family == 6 && model < 3 && stepping < 3) return 0;
			return 1;

		case KCPUF_SYSCALL:
			if(!amd) return 0;
			if(!(ef_edx & CPUID_EF_EDX_SYSENTER)) return 0;
			return 1;
    }

    return 0;
}