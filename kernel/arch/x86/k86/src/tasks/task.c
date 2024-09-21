#define __ARCH_X86_INTERNALS__ 1
#include <x86/tasks/task.h>
#include <x86/k86/tables.h>
#include <x86/k86/taskseg.h>
#include <x86/k86/paging.h>
#include <x86/k86/irq.h>
#include <x86/klib.h>
#include <x86/smp.h>

#include <x86/peripherals/apic.h>

#include <zeronix/kstring.h>

// --
extern void task_far_jump(uint32_t ip, uint16_t cs);
extern void task_switch_swint();

// --> dummy tasks.
karch_task_t task_dummy[MAX_CPU] __aligned(8);

// --
void karch_task_dummy_init() {
    kmemset(&task_dummy, 0, sizeof(task_dummy));
}

// --
void karch_task_init(karch_task_t* task) {
    kmemset(task, 0, sizeof(karch_task_t));
    int32_t cpu = karch_smp_cpuid();
    if (cpu < 0) {
        cpu = 0;
    }

    karch_stackmark_t* sm = karch_taskseg_get_stackmark(cpu);
    karch_tss_t* tss = &task->tss;
    
    task->type = 0;
    task->cpu = -1;

    // --> set interrupt stack pointer and its segment.
    tss->ss0 = SEG_SEL(GDT_KERN_DS);
    tss->sp0 = sm;

    // --> code & data segments to kernel segment.
    tss->cs = SEG_SEL(GDT_KERN_CS);
    tss->ds = tss->es = tss->fs = 
    tss->gs = tss->ss = SEG_SEL(GDT_KERN_DS);

    tss->cr3 = PAGE_DIR_ADDR;
    tss->iobase = sizeof(karch_tss_t);
}

void karch_task_set_user(karch_task_t* task) {
    karch_tss_t* tss = &task->tss;

    // --> code & data segments to kernel segment.
    tss->cs = SEG_SEL(GDT_USER_CS);
    tss->ds = tss->es = tss->fs = 
    tss->gs = tss->ss = SEG_SEL(GDT_USER_DS);
}

void karch_task_set_exec(
    karch_task_t* task, karch_vaddr_t ip, karch_vaddr_t sp)
{
    task->tss.ip = ip;
    task->tss.sp = sp;   
}

void karch_task_switch_to(karch_task_t* task) {
    int32_t cpu = karch_smp_cpuid();
    if (cpu < 0) {
        cpu = 0;
    }

    // --> set process.
    karch_stackmark_t* sm
        = karch_taskseg_get_stackmark(cpu);

    karch_tss_t* tss
        = karch_taskseg_get(cpu);

    // --> set dummy as prev task if missing.
    if (!sm->prev) {
        sm->prev = &task_dummy[cpu];
    }

    // --> set the next task.
    sm->next = task;

    // --> if under interrupt frame, 
    //   : no need to trigger switching interrupt.
    if (karch_irq_get_frame()) {
        return;
    }

    // load_tr(SEG_TSS(cpu));
    *tss = task->tss;
    task_far_jump(0, SEG_TSS(cpu));

        //task_switch_swint();
    if (!karch_smp_supported()) {
        return;
    }

    karch_lapic_emit_ipi(
        0xf1, cpu, LAPICIPI_SPECIFIC);

    // --> wait for ipi execution.
    while(1) {
        cpu_hlt();
    }
}

