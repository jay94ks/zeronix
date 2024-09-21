#define __ARCH_X86_INTERNALS__
#include <x86/tasks/task.h>
#include <x86/k86/tables.h>
#include <x86/k86/taskseg.h>
#include <x86/k86/paging.h>
#include <x86/env.h>
#include <x86/klib.h>

#include <zeronix/kstring.h>

// --
karch_tss_t tss[MAX_CPU] __aligned(8);
karch_stackmark_t* tss_sm[MAX_CPU] __aligned(8);

// --
extern void* _karch_tss0_stack;

// --
#define INTEL_MSR_SYSENTER_CS           0x174
#define INTEL_MSR_SYSENTER_ESP          0x175
#define INTEL_MSR_SYSENTER_EIP          0x176

#define AMD_EFER_SCE		            (1L << 0) /* SYSCALL/SYSRET enabled */
#define AMD_MSR_EFER		            0xC0000080	/* extended features msr */
#define AMD_MSR_STAR		            0xC0000081	/* SYSCALL params msr */

// --
void karch_taskseg_init() {
    kmemset(tss, 0, sizeof(tss));
    kmemset(tss_sm, 0, sizeof(tss_sm));

    karch_taskseg_setup(0, 0);
}

karch_tss_t* karch_taskseg_get(uint8_t n) {
    if (n >= MAX_CPU) {
        return 0;
    }

    return &tss[n];
}

void* karch_taskseg_get_stacktop(uint8_t n) {
    if (n >= MAX_CPU) {
        return 0;
    }

    return (((uint8_t*)&_karch_tss0_stack) - (n * I686_PAGE_SIZE));
}

karch_stackmark_t* karch_taskseg_get_stackmark(uint8_t n) {
    if (n >= MAX_CPU) {
        return 0;
    }

    return tss_sm[n];
}

uint8_t karch_taskseg_setup(uint8_t n, void* stack_top) {
    karch_tss_t* tss = karch_taskseg_get(n);
    karch_seg_t* seg = karch_tables_gdt_ptr(GDT_TSS_INDEX(n));
 
    if (!tss || !seg) {
        return 0;
    }

    if (!stack_top && !(stack_top = karch_taskseg_get_stacktop(n))) {
        return 0;
    }
    
    karch_stackmark_t* sm = (karch_stackmark_t*)(
        (uint8_t*)stack_top - sizeof(karch_stackmark_t));

    karch_tables_gdt_data(seg, (uint32_t)tss, sizeof(karch_tss_t), PRIV_KERN);
    seg->access = PRESENT | (PRIV_KERN << DPL_SHIFT) | TSS_TYPE;

    kmemset(tss, 0, sizeof(karch_tss_t));
    tss->ds = tss->es = tss->fs = tss->gs = tss->ss = tss->ss0 = SEG_SEL(GDT_KERN_DS);
    tss->cs = SEG_SEL(GDT_KERN_CS);
    tss->iobase = sizeof(karch_tss_t);
    tss->sp0 = (uint32_t)(tss_sm[n] = sm);
    
    sm->cpu = n;
    sm->prev = sm->next = 0;

    uint8_t syscall_impl = karch_env_syscall_type();
    if ((syscall_impl & ENV_USE_SYSENTER) != 0) {
        write_msr(INTEL_MSR_SYSENTER_CS, 0, SEG_SEL(GDT_KERN_CS));
        write_msr(INTEL_MSR_SYSENTER_ESP, 0, tss->sp0);
        write_msr(INTEL_MSR_SYSENTER_EIP, 0, 0 /* TODO: IPC_ENTRY_SYSENTER */);
    }

    if ((syscall_impl & ENV_USE_SYSCALL) != 0) {
        uint32_t msr_lo, msr_hi;

        read_msr(AMD_MSR_EFER, &msr_hi, &msr_lo);
        msr_lo |= (1 << 0);

        write_msr(AMD_MSR_EFER, msr_hi, msr_lo);
        write_msr(AMD_MSR_STAR, 
            (SEG_SEL(GDT_USER_CS) | PRIV_USER) << 16 | SEG_SEL(GDT_KERN_CS),
            0 /* todo: ipc_entry_stscall_ ## cpu */
        );
    }

    return 1;
}