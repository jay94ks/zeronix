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
//extern void task_far_jump(uint32_t ip, uint16_t cs);
extern void task_switch_swint();

// --> kernel tasks.
karch_task_t task_kern[MAX_CPU] __aligned(16);
karch_spinlock_t task_lock;

// --
void karch_task_kerns_init() {
    kmemset(&task_kern, 0, sizeof(task_kern));
    karch_spinlock_init(&task_lock);
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

uint8_t karch_task_set_pagedir(karch_task_t* task, karch_pagedir_t pagedir) {
    karch_tss_t* tss = &task->tss;

    if (!karch_paging_verify_pagedir(pagedir)) {
        return 0;
    }

    tss->cr3 = (uint32_t)(pagedir);
    return 1;
}

void karch_task_set_exec(
    karch_task_t* task, karch_vaddr_t ip, karch_vaddr_t sp)
{
    task->tss.ip = ip;
    task->tss.sp = sp;   
}

karch_tsret_t karch_task_switch(karch_task_t* task) {
    int16_t n = karch_smp_cpuid();
    if (n < 0) {
        n = 0;
    }

    return karch_task_switch_to_cpu(n, task);
}

karch_tsret_t karch_task_switch_to_cpu(uint8_t n, karch_task_t* task) {
    // --> set process.
    volatile karch_stackmark_t* sm
        = karch_taskseg_get_stackmark(n);
        
    karch_tss_t* tss = karch_taskseg_get(n);

    if (!sm || !tss) {
        return KTSRET_NO_CPU;
    }

    if (!karch_spinlock_trylock(&task_lock)) {
        return KTSRET_BUSY;
    }

    if (sm->prev == task) {
        karch_spinlock_unlock(&task_lock);
        return KTSRET_ALREADY;
    }

    else {
        // --> set dummy as prev task if missing.
        if (!sm->prev) {
            sm->prev = &task_kern[n];
        }

        // --> set the next task.
        sm->next = task;
    }

    karch_spinlock_unlock(&task_lock);

    // --> if under interrupt frame, 
    //   : no need to trigger switching interrupt.
    if (karch_irq_get_frame()) {
        return KTSRET_OKAY;
    }

    if (!karch_smp_supported()) {
        task_switch_swint();
        return KTSRET_OKAY;
    }

    karch_lapic_emit_ipi(0xf1, n, LAPICIPI_SPECIFIC);
    return KTSRET_OKAY;
}

