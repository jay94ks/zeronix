#ifndef __INC_ZERONIX_ARCH_TASK_H__
#define __INC_ZERONIX_ARCH_TASK_H__

#include "x86/taskseg.h"

/**
 * This provides `TASK` interface that runs on physical processor.
 */

#ifdef __cplusplus
extern "C" {
#endif
/**
 * defines how a task looks like.
 * usage:
 * ---
 * struct my_proc {
 *     karch_task_t task; // --> this must be first.
 * 
 * } __packed;
 * 
 * then, call task interface like: (karch_task_t*)&proc.
 */
struct __karch_task_t {
    karch_fpu_t fpu;        // --> how FPU should be set.
    karch_tss_t tss;        // --> how the task looks like.
    uint32_t type;          // --> type field.
    int16_t cpu;            // --> which CPU runs this task.
    uint8_t trap;           // --> if the task is visited to interrupt, remember how it get called.
    uint8_t rsv;
} __aligned(16);

typedef struct __karch_task_t karch_task_t;

#ifdef __cplusplus
}
#endif

#endif