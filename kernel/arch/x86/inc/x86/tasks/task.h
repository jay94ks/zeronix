#ifndef __KERNEL_ARCH_X86_INC_X86_TASKS_TASK_H__
#define __KERNEL_ARCH_X86_INC_X86_TASKS_TASK_H__

#include <zeronix/arch/task.h>
#include <zeronix/arch/paging.h>
#include <x86/k86/taskseg.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * initialize the task structure to default.
 */
void karch_task_init(karch_task_t* task);

/**
 * set the task as user task.
 */
void karch_task_set_user(karch_task_t* task);

/**
 * set the page directory for the task.
 * this returns zero if page directory couldn't be verified.
 */
uint8_t karch_task_set_pagedir(karch_task_t* task, karch_pagedir_t pagedir);

/**
 * set the task's execution target.
 */
void karch_task_set_exec(
    karch_task_t* task, karch_vaddr_t ip, karch_vaddr_t sp);

/**
 * task switch return values.
 */
typedef enum {
    KTSRET_OKAY = 0,    // --> Okay, it'll be executed soon.
    KTSRET_ALREADY,     // --> the specified task is already reserved.
    KTSRET_BUSY,        // --> the task scheduler is busy yet.
    KTSRET_NO_CPU,      // --> no such CPU number exists.
} karch_tsret_t;

/**
 * switch the current running CPU's task to specified task.
 */
karch_tsret_t karch_task_switch(karch_task_t* task);

/**
 * switch specified CPU's task to specified task.
 */
karch_tsret_t karch_task_switch_to_cpu(uint8_t n, karch_task_t* task);
#ifdef __cplusplus
}
#endif
#endif