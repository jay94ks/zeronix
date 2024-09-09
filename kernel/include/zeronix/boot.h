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
 */
typedef enum {
    KBOOTMMAP_UNAVAILABLE = 0,
    KBOOTMMAP_AVAILABLE = 1,
} kbootinfo_mmap_type_t;

/**
 * kbootinfo_mmap_t.
 */
typedef struct {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type; // --> kbootinfo_mmap_type_t
} kbootinfo_mmap_t;

// --> maximum entities of kbootinfo_mmap_t.
#define KBOOT_MAX_MEMMAP    32

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