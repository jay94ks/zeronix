#include "tss.h"
#include "gdt.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/layout.h>
#include <zeronix/arch/x86/cpuinfo.h>
#include <zeronix/arch/x86/cpu.h>

// --
uint8_t syscall_impl = 0;

enum {
    USE_SYSENTER = 1,
    USE_SYSCALL = 2
};

void karch_init_tss() {
    syscall_impl = 0;
    syscall_impl |= kcpuinfo(KCPUF_SYSENTER)? USE_SYSENTER : 0;
    syscall_impl |= kcpuinfo(KCPUF_SYSCALL) ? USE_SYSCALL : 0;

    // --> CPU zero.
    karch_setup_tss(0, karch_stacktop_for(0));
}

uint8_t karch_setup_tss(uint8_t cpu, void* stack_top) {
    karch_cpu_t* cpu_ = karch_get_cpu(cpu);

    karch_tss_t* tss = &cpu_->tss;
    karch_seg_t* seg = karch_get_gdt(GDT_TSS_INDEX(cpu));

    if (!seg || !stack_top) {
        return 0;
    }
    
    karch_stackmark_t* sm = (karch_stackmark_t*)(
        (uint8_t*)stack_top - sizeof(karch_stackmark_t));

    karch_set_gdt_data(seg, (uint32_t)tss, sizeof(karch_tss_t), PRIV_KERN);
    seg->access = PRESENT | (PRIV_KERN << DPL_SHIFT) | TSS_TYPE;

    kmemset(tss, 0, sizeof(karch_tss_t));
    tss->ds = tss->es = tss->fs = tss->gs = tss->ss0 = GDT_KERN_DS;
    tss->cs = GDT_KERN_CS;
    tss->iobase = sizeof(karch_tss_t);
    
    tss->sp0 = (uint32_t)(cpu_->stackmark = sm);
    cpu_->flags |= CPUFLAG_INIT_TSS;

    sm->cpu = cpu;
    sm->proc = 0;

    if ((syscall_impl & USE_SYSENTER) != 0) {
        write_msr(INTEL_MSR_SYSENTER_CS, 0, SEG_SEL(GDT_KERN_CS));
        write_msr(INTEL_MSR_SYSENTER_ESP, 0, tss->sp0);
        write_msr(INTEL_MSR_SYSENTER_EIP, 0, 0 /* TODO: IPC_ENTRY_SYSENTER */);
    }

    if ((syscall_impl & USE_SYSCALL) != 0) {
        uint32_t msr_lo, msr_hi;

        read_msr(AMD_MSR_EFER, &msr_hi, &msr_lo);
        msr_lo |= (1 << 0);

        write_msr(AMD_MSR_EFER, msr_hi, msr_lo);
        write_msr(AMD_MSR_STAR, 
            (SEG_SEL(GDT_USER_CS) | PRIV_USER) << 16 | SEG_SEL(GDT_KERN_CS),
            0 /* todo: ipc_entry_stscall_ ## cpu */
        );
    }
}