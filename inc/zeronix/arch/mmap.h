#ifndef __INC_ZERONIX_ARCH_MMAP_H__
#define __INC_ZERONIX_ARCH_MMAP_H__

#include <zeronix/types.h>

/**
 * This header provides memory map about architecture.
 */
// --
#define MMAP_MAX    32

/**
 * represents memory region.
 */
typedef struct {
    uint32_t addr;
    uint32_t len;
} karch_mmap_elem_t;

/**
 * memory map.
 */
typedef struct {
    uint32_t count;
    uint32_t total;
    karch_mmap_elem_t mmap[MMAP_MAX];
} karch_mmap_t;

#endif