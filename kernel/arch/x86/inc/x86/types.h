#ifndef __KERNEL_ARCH_X86_INC_X86_TYPES_H__
#define __KERNEL_ARCH_X86_INC_X86_TYPES_H__

#include <zeronix/types.h>
#include <zeronix/arch/irq.h>
#include "i686.h"

/* meaningful aliases. */
typedef uint32_t karch_reg32_t;
typedef uint16_t karch_reg16_t;

/* aliases. */
typedef karch_irq_frame_t karch_intr_frame_t;
typedef karch_irq_regs_t karch_intr_regs_t;

// --> cast to interrupt frame from exception frame.
#define KARCH_TO_INTR_FRAME(exfr) ((karch_intr_frame_t*)((uint8_t*)exfr + sizeof(karch_reg32_t)))

#endif