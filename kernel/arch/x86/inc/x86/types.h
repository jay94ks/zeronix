#ifndef __KERNEL_ARCH_X86_INC_X86_TYPES_H__
#define __KERNEL_ARCH_X86_INC_X86_TYPES_H__

#include <zeronix/types.h>
#include "i686.h"

/* meaningful aliases. */
typedef uint32_t karch_reg32_t;
typedef uint16_t karch_reg16_t;

/**
 * karch_x86_intr_frame_t
 * interrupt frame.
 */
struct karch_x86_intr_frame_t {
    karch_reg32_t err;  // --> if zero, not an error.

    /* hardware defined. */
    karch_reg32_t ip;   // 12
    karch_reg32_t cs;   // 16
    karch_reg32_t flags;

    /* only for crossing priv. */
    karch_reg32_t sp;
    karch_reg32_t ss;
} __packed;

/* short hand. */
typedef struct karch_x86_intr_frame_t karch_intr_frame_t;

// --> cast to interrupt frame from exception frame.
#define KARCH_TO_INTR_FRAME(exfr) ((karch_intr_frame_t*)((uint8_t*)exfr + sizeof(karch_reg32_t)))

#endif