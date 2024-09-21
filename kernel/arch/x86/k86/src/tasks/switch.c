#define __ARCH_X86_INTERNALS__ 1
#include <x86/tasks/task.h>
#include <x86/k86/tables.h>
#include <x86/k86/irq.h>
#include <x86/klib.h>
#include <x86/smp.h>

#include <x86/peripherals/fpu.h>
#include <zeronix/kstring.h>

void karch_task_switch_intr(
    karch_intr_frame_t* frame,
    karch_intr_regs_t* regs) 
{
    int32_t cpu = karch_smp_cpuid();
    if (cpu < 0) {
        cpu = 0;
    }

    load_kern_cs();
    load_ds(SEG_SEL(GDT_KERN_DS));
    load_es(SEG_SEL(GDT_KERN_DS));
    load_fs(SEG_SEL(GDT_KERN_DS));
    load_gs(SEG_SEL(GDT_KERN_DS));
    load_ss(SEG_SEL(GDT_KERN_DS));
    
    // --> kernel to kernel interrupt.
    /*if (frame->cs == SEG_KERN_CS) {
        return;
    }*/

    volatile karch_stackmark_t* sm
        = karch_taskseg_get_stackmark(cpu);

    // --> fetch tasks to run.
    karch_task_t* prev = sm->prev;
    karch_task_t* next = sm->next;

    // --> if no task switch reserved, skip this.
    if (next == prev) {
        return;
    }

    // --> store previous task state.
    if (prev) {
        karch_tss_t* tss = &prev->tss;
        karch_fpu_store(&prev->fpu);

        tss->ip = frame->ip;
        tss->ss = frame->ss;
        tss->cs = frame->cs;

        tss->ax = regs->ax;
        tss->bx = regs->bx;
        tss->cx = regs->cx;
        tss->dx = regs->dx;

        tss->flags = regs->flags;
        tss->es = regs->es;
        tss->ds = regs->ds;
        tss->ss = regs->ss;
        tss->fs = regs->fs;
        tss->gs = regs->gs;
        tss->cr3 = regs->cr3;
    }

    // --> restore next task state.
    if (next) {
        karch_tss_t* tss = &next->tss;
        karch_fpu_restore(&next->fpu);

        frame->ip = tss->ip;
        frame->ss = tss->ss;
        frame->cs = tss->cs;

        regs->ax = tss->ax;
        regs->bx = tss->bx;
        regs->cx = tss->cx;
        regs->dx = tss->dx;

        regs->flags = tss->flags;
        regs->es = tss->es;
        regs->ds = tss->ds;
        regs->ss = tss->ss;
        regs->fs = tss->fs;
        regs->gs = tss->gs;
        regs->cr3 = tss->cr3;
    }

    // --> set current next as prev.
    sm->prev = sm->next;
}