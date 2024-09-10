#include "i8253.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>

#define i8253_REG_MODE      0x43
#define i8253_REG_FREA      0x40
#define i8253_SQUARE_WAVE   0x36

#define i8253_FREQ_DIV      1193182
#define i8253_TIMER_CNT(x)  (i8253_FREQ_DIV / (x))

/**
 * initialize i8253 timer.
 */
void karch_init_i8253(uint32_t freq) {
    cpu_out8(i8253_REG_MODE, i8253_SQUARE_WAVE);

    if (freq == 0) {
        cpu_out8(i8253_REG_FREA, 0);
        cpu_out8(i8253_REG_FREA, 0);
        return;
    }

    freq = i8253_TIMER_CNT(freq);
    cpu_out8(i8253_REG_FREA, freq & 0xff);
    cpu_out8(i8253_REG_FREA, (freq >> 8) & 0xff);
}

void karch_deinit_i8253() {
    karch_init_i8253(0);
}