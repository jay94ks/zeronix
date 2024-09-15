#ifndef __ZERONIX_BOOT_H__
#define __ZERONIX_BOOT_H__

#include <zeronix/types.h>

/**
 * kbootmem_t.
 */
typedef struct {
    uint32_t addr;
    uint32_t len;
} kbootmem_t;

/**
 * kbootinfo_mmap_type_t
 * this'll be just copied from multiboot.
 */
typedef enum {
    KBOOTMMAP_UNKNOWN = 0,
    KBOOTMMAP_AVAIL = 1,
    KBOOTMMAP_RESERVED = 2,
    KBOOTMMAP_ACPI = 3,
    KBOOTMMAP_NVS = 4,
    KBOOTMMAP_BADRAM = 5,
} kbootinfo_mmap_type_t;

/**
 * kbootinfo_mmap_t.
 */
typedef struct {
    uint32_t addr;
    uint32_t len;
    uint32_t type; // --> kbootinfo_mmap_type_t
} kbootinfo_mmap_t;

// --> maximum entities of kbootinfo_mmap_t.
#define KBOOT_MAX_MEMMAP    64

/**
 * kbootinfo_t.
 */
typedef struct {
    uint32_t mem_high_phys;

    // --> kernel address in virtual world.
    uint32_t kern_virt_base;
    kbootmem_t kern_virt;

    // --> bootstrap region.
    kbootmem_t bootstrap;
    
    // --> kernel allocated memory. 
    uint32_t kern_alloc;
    uint32_t kern_alloc_dyn;

    // --> memory map.
    kbootinfo_mmap_t mmap[KBOOT_MAX_MEMMAP];
    uint32_t mmap_cnt;

    // --> paging stats.
    uint32_t* pagedir;
    uint32_t freepde;
} kbootinfo_t;

#endif