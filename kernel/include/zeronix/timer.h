#ifndef __ZERONIX_TIMER_H__
#define __ZERONIX_TIMER_H__

#include <zeronix/types.h>
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * Kernel timer callback type.
 */
typedef void (* ktimer_func_t)(void*);

/**
 * Kernel timer's time type.
 */
typedef uint32_t ktimer_time_t;

/**
 * Kernel timer.
 * to rearm the timer, reset `exp`.
 */
typedef struct ktimer {
    struct ktimer*  next;   // --> next link.
    ktimer_time_t   exp;    // --> expires after.
    ktimer_func_t   func;   // --> func ptr.
    void*           data;   // --> data ptr.
} ktimer_t;

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif