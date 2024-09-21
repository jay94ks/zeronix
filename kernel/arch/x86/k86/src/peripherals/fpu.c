#define __ARCH_X86_INTERNALS__ 1

#include <x86/peripherals/fpu.h>
#include <x86/peripherals/apic.h>
#include <x86/k86/irq.h>
#include <x86/k86/tables.h>
#include <x86/env.h>
#include <x86/klib.h>

// --
extern void* __fpu_error_eip_start;
extern void* __fpu_error_eip_end;
extern void* __fpu_error_eip_patch;

#define fpu_error_eip_start     ((uint32_t)&__fpu_error_eip_start)
#define fpu_error_eip_end       ((uint32_t)&__fpu_error_eip_end)
#define fpu_error_eip_patch     ((uint32_t)&__fpu_error_eip_patch)

// --
extern void cpu_fninit();
extern uint16_t cpu_fnstsw();
extern void cpu_fnstcw(void*);
extern void cpu_fnsave(void*);
extern void cpu_fxsave(void*);
extern void cpu_frstor(void*);
extern void cpu_fxrstor(void*);

#define CR0_MP_NE	    0x0022      /* set MP and NE flags to handle FPU exceptions in native mode. */
#define CR4_OSFXSR	    (1L<<9)     /* set CR4.OSFXSR[bit 9] if FXSR is supported. */
#define CR4_OSXMMEXCPT	(1L<<10)    /* set OSXMMEXCPT[bit 10] if we provide #XM handler. */

// --
// fxrstor, frstor: exception: #GP, #SS, #NM, #PF, #AC, #UD
//   * #PF is not needed to handle on here.
karch_irq_t 
    irq_fpu_gp, irq_fpu_ss, irq_fpu_nm, 
    irq_fpu_ud, irq_fpu_ac;

uint8_t fpu_flags[MAX_CPU]; // --> 0x01: FPU present, 0x02: OSFXSR, 0x04: SSE

/**
 * if FPU instruction error occured, 
 * this patches EIP to return error value.
 */
void karch_fpu_except(karch_irq_t* irq) {
    karch_intr_frame_t* intr = karch_irq_get_frame();

    // --> check error range here.
    if (intr->ip >= fpu_error_eip_start &&
        intr->ip <= fpu_error_eip_end)
    {
        intr->ip = fpu_error_eip_patch;
        return;
    }
}

// --
void karch_fpu_setup_irq();

// --
void karch_fpu_init() {
    const int16_t lapic_n = karch_lapic_number();
    const karch_lapic_t* lapic = karch_lapic_get_current();
    const uint8_t is_bsp = !lapic || lapic->bsp;

    uint8_t* local_flags = &fpu_flags[lapic_n < 0 ? 0 : lapic_n];
    uint16_t sw, cw;

    // --> setup IRQ if under BSP.
    if (is_bsp) {
        karch_fpu_setup_irq();
    }
    
    // --> clear flags.
    *local_flags = 0;

	cpu_fninit();
	sw = cpu_fnstsw();
	cpu_fnstcw(&cw);

	if((sw & 0xff) == 0 && (cw & 0x103f) == 0x3f) {
        karch_cpuinfo_t info;
        
        // --> set CR0_MP_NE and CR0_MP_MP to handle FPU exception in native mode.
		write_cr0(read_cr0() | CR0_MP_NE);
        *local_flags |= KFPUFL_EXISTS;

        // --> no CPUID feature.
        if (!karch_env_cpuinfo(&info)) {
            return;
        }

        // --> enable FXSR if supported.
		if((info.feature[CPUINFO_EDX] & CPUID1_EDX_FXSR) != 0) {
			uint32_t cr4 = read_cr4() | CR4_OSFXSR; /* Enable FXSR. */
            *local_flags |= KFPUFL_OSFXSR;

            // --> enable CR4_OSXMMEXCPT if supported.
			if((info.feature[CPUINFO_EDX] & CPUID1_EDX_SSE) != 0) {
				cr4 |= CR4_OSXMMEXCPT; 
                *local_flags |= KFPUFL_SSE;
            }

			write_cr4(cr4);
        }
	}
}

void karch_fpu_setup_irq() {
    karch_irq_t temp;
    temp.handler = karch_fpu_except;

    // all of IRQs will be handled by same handler.
    irq_fpu_gp = 
    irq_fpu_ss = 
    irq_fpu_nm =
    irq_fpu_ud = 
    irq_fpu_ac = temp;
    
    karch_irq_register(IRQN_EXC_PROTECTION, &irq_fpu_gp);
    karch_irq_register(IRQN_EXC_STACK_FAULT, &irq_fpu_ss);
    karch_irq_register(IRQN_EXC_COPR_NAVL, &irq_fpu_nm);
    karch_irq_register(IRQN_EXC_INVAL_OPCODE, &irq_fpu_ud);
    karch_irq_register(IRQN_EXC_ALIGNMENT, &irq_fpu_ac);
}

uint8_t karch_fpu_get_flags() {
    const int16_t lapic_n = karch_lapic_number();
    return fpu_flags[lapic_n < 0 ? 0 : lapic_n];
}

uint8_t karch_fpu_store(karch_fpu_t* state) {
    // --> has no fpu.
    if (!karch_fpu_exists()) {
        return 0;
    }

    if (karch_fpu_osfxsr()) {
        cpu_fxsave(state->raw);
    }
    else {
        cpu_fnsave(state->raw);
    }

    return 1;
}

uint8_t karch_fpu_restore(const karch_fpu_t* state) {
    // --> has no fpu.
    if (!karch_fpu_exists()) {
        return 0;
    }

    if (karch_fpu_osfxsr()) {
        cpu_fxrstor(state->raw);
    }
    else {
        cpu_frstor(state->raw);
    }
    
    return 1;
}