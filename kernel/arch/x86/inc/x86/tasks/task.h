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
 * set the task's execution target.
 */
void karch_task_set_exec(
    karch_task_t* task, karch_vaddr_t ip, karch_vaddr_t sp);

/**
 * switch to specified task.
 */
void karch_task_switch_to(karch_task_t* task) ;
#ifdef __cplusplus
}
#endif
#endif