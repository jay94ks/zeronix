#ifndef __KERNEL_ARCH_X86_INC_X86_BOOT_BOOTINFO_H__
#define __KERNEL_ARCH_X86_INC_X86_BOOT_BOOTINFO_H__

#include <zeronix/types.h>
#include "multiboot.h"

/**
 * memory map element type.
 * this'll be just copied from multiboot.
 */
typedef enum {
    BOOTMMAP_UNKNOWN = 0,
    BOOTMMAP_AVAIL = 1,
    BOOTMMAP_RESERVED = 2,
    BOOTMMAP_ACPI = 3,
    BOOTMMAP_NVS = 4,
    BOOTMMAP_BADRAM = 5,
} bootinfo_mtype_t;

/**
 * memory map element.
 */
typedef struct {
    uint32_t addr;
    uint32_t len;
    uint32_t type; // --> bootinfo_mtype_t
} bootinfo_mmap_t;

// --> max element for memory map.
#define BOOTINFO_MAX_MEMMAP 64

/**
 * boot information.
 */
typedef struct {
    uint32_t mem_high_phys;
    uint32_t mmap_n;
    bootinfo_mmap_t mmap[BOOTINFO_MAX_MEMMAP];
} bootinfo_t;

#endif