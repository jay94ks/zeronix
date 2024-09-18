#ifndef __INC_ZERONIX_ARCH_TIMER_H__
#define __INC_ZERONIX_ARCH_TIMER_H__

#include <zeronix/types.h>

/* timer interrupt callback: no args. */
typedef void(* karch_timer_cb_t)();

/**
 * architecture specific hardware timer interface.
 * note that, timer interrupt is only triggered at BSP.
 */
typedef struct {
    /* get the timer handler. */
    karch_timer_cb_t (* get_handler)();

    /* set the timer handler. */
    void (* set_handler)(karch_timer_cb_t* cb);

    /* start timer. */
    uint8_t (* start)(uint32_t freq);

    /* stop timer. */
    uint8_t (* stop)();
} karch_timer_t;

#endif