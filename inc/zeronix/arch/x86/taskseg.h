#ifndef __INC_ZERONIX_ARCH_X86_TASKSEG_H__
#define __INC_ZERONIX_ARCH_X86_TASKSEG_H__

#include <zeronix/types.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * karch_x86_tss_t.
 * task descriptor.
*/
struct karch_x86_tss_t {    // off      stack
    uint32_t backlink;      // 0        100
    uint32_t sp0;           // 4        96
    uint32_t ss0;           // 8        92
    uint32_t sp1;           // 12       88
    uint32_t ss1;           // 16       84
    uint32_t sp2;           // 20       80
    uint32_t ss2;           // 24       76
    uint32_t cr3;           // 28       72  v
    uint32_t ip;            // 32       68
    uint32_t flags;         // 36       64  v
    uint32_t ax;            // 40       60  v
    uint32_t cx;            // 44       56  v
    uint32_t dx;            // 48       52  v
    uint32_t bx;            // 52       48  v
    uint32_t sp;            // 56       44  v
    uint32_t bp;            // 60       40  v
    uint32_t si;            // 64       36  v
    uint32_t di;            // 68       32  v
    uint32_t es;            // 72       28  v
    uint32_t cs;            // 76       24  
    uint32_t ss;            // 80       20  v
    uint32_t ds;            // 84       16  v
    uint32_t fs;            // 88       12  v
    uint32_t gs;            // 92       8   v
    uint32_t ldt;           // 96       4
    uint16_t trap;          // 98       2
    uint16_t iobase;        // 100      0
} __packed;

typedef struct karch_x86_tss_t karch_tss_t;

/**
 * FPU state size.
 */
#define KARCH_FPU_STATE_MAX    512

/**
 * FPU state.
 */
struct karch_x86_fpu_t {
    uint8_t raw[KARCH_FPU_STATE_MAX];
} __packed;

typedef struct karch_x86_fpu_t karch_fpu_t;

#ifdef __cplusplus
}
#endif
#endif