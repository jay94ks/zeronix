#ifndef __INC_ZERONIX_ARCH_PAGING_H__
#define __INC_ZERONIX_ARCH_PAGING_H__
#include <zeronix/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * page directory flags.
 * this has same value with `x86`.
 */
enum {
    /**
     * get or set whether the virtual address is mapped or not.
     */
    KPAGEDIR_PRESENT       = 0x001,

    /**
     * get or set whether the virtual address is writable or not.
     */
    KPAGEDIR_WRITABLE      = 0x002,

    /**
     * get or set whether the page directory's entry
     * is working as redirector to the page table or not.
     */
    KPAGEDIR_BIG_PAGE      = 0x080,

    /**
     * get or set whether the page TLB clear or not.
     * if this set, when page directory is switching, TLB will not be cleared. 
     */
    KPAGEDIR_GLOBAL        = 0x100,

    /**
     * get or set whether the users can access the virtual address or not.
     */
    KPAGEDIR_USER_ACCESS   = 0x004,

    /**
     * get or set whether the virtual address can not be cached or not.
     */
    KPAGEDIR_NO_CACHING    = 0x010,

    /**
     * write through.
     */
    KPAGEDIR_WRITE_THROUGH = 0x008,

    /**
     * get or set whether the virtual address is for hardware IO access or not.
     */
    KPAGEDIR_IO_MAPPING    = KPAGEDIR_NO_CACHING | KPAGEDIR_WRITE_THROUGH,

    /**
     * set by hardware or compatible layer.
     * indicates whether the page is accessed or not.
     */
    KPAGEDIR_ACCESSED      = 0x020,

    /**
     * set by hardware or compatible layer.
     * indicates whether the page is dirty (written something) or not.
     */
    KPAGEDIR_DIRTY         = 0x040,
};

/**
 * page table flags.
 * this has same value with `x86`.
 */
enum {
    KPAGETBL_PRESENT       = KPAGEDIR_PRESENT,

    /**
     * get or set whether the virtual address is writable or not.
     */
    KPAGETBL_WRITABLE      = KPAGEDIR_WRITABLE,

    /**
     * get or set whether the page TLB clear or not.
     * if this set, when page directory is switching, TLB will not be cleared. 
     */
    KPAGETBL_GLOBAL        = KPAGEDIR_GLOBAL,

    /**
     * get or set whether the users can access the virtual address or not.
     */
    KPAGETBL_USER_ACCESS   = KPAGEDIR_USER_ACCESS,

    /**
     * get or set whether the virtual address can not be cached or not.
     */
    KPAGETBL_NO_CACHING    = KPAGEDIR_NO_CACHING,

    /**
     * write through.
     */
    KPAGETBL_WRITE_THROUGH = KPAGEDIR_WRITE_THROUGH,

    /**
     * get or set whether the virtual address is for hardware IO access or not.
     */
    KPAGETBL_IO_MAPPING    = KPAGEDIR_NO_CACHING | KPAGEDIR_WRITE_THROUGH,

    /**
     * set by hardware or compatible layer.
     * indicates whether the page is accessed or not.
     */
    KPAGETBL_ACCESSED      = KPAGEDIR_ACCESSED,

    /**
     * set by hardware or compatible layer.
     * indicates whether the page is dirty (written something) or not.
     */
    KPAGETBL_DIRTY         = KPAGEDIR_DIRTY,
};

/* page directory pointer type. */
typedef struct { uint8_t  _[4]; }   * karch_pagedir_t;

/* page table pointer type. */
typedef struct { uint16_t _[2]; }   * karch_pagetbl_t;

/* physical address type. */
typedef struct { uint16_t _; }      * karch_paddr_t;

/* virtual address type. */
typedef struct { uint32_t _; }      * karch_vaddr_t;

/* physical address offset type. */
typedef uint32_t                      karch_paddr_off_t;

/* cast to physical address type. */
#define AS_PHYS(x)                  ((karch_paddr_t)(x))

/* cast to virtual address type. */
#define AS_VIRT(x)                  ((karch_vaddr_t)(x))

/**
 * virtual to physical address/physical to virtual address translation result.
 * 1) virtual to physical: set `vaddr` and `vlen`.
 *    --> result'll set on `paddr` and `plen`.
 *        if `vlen` != `plen`: not linear accessible.
 *        otherwise, linear accessible.
 * 
 * 2) physical to virtual: set `paddr` to `plen`.
 *    --> result'll set on `vaddr` and `vlen`.
 *        if `vlen` != `plen`: not linear accessible.
 *        otherwise, linear accessible.
 * 
 * 3) if no `plen` or `vlen` set, aka, zero:
 *    --> in this case, this will translate address only.
 */
typedef struct {
    karch_vaddr_t vaddr;
    uint32_t      vlen;

    karch_paddr_t paddr;
    uint32_t      plen;
} karch_paging_trans_t;

/**
 * paging interface.
 * note that, all pointers, pagedir, pagetbl must be physical address space.
 * note that, all set_pagedir_* and set_pagedir_* will trim address automatically.
 * 
 * 1. given address is not aligned on required page alignment: fail.
 * 2. this is designed based on `X86`. 
 *    so, other architecture's implementations must be compatible with it.
 *    (even including software-way)
 */
typedef struct {
    /* size of normal page. */
    uint32_t page_size;

    /* big page size. */
    uint32_t big_page_size;

    /* alignment for pages. */
    uint32_t page_align;

    /* alignments for big pages. */
    uint32_t big_page_align;

    /* total bytes to construct page directory. */
    uint32_t page_directory_size;

    /* total bytes to construct each page table. */
    uint32_t page_table_size;

    /* verify page directory is located at correct location or not. */
    uint8_t (* verify_pagedir)(karch_pagedir_t);

    /* verify page table is located at correct location or not. */
    uint8_t (* verify_pagetbl)(karch_pagetbl_t);

    /* set the page table to page directory entry. */
    uint8_t (* set_pagetbl)(karch_pagedir_t, karch_vaddr_t, karch_pagetbl_t, uint32_t flags);

    /* set the physical address to page directory entry. */
    uint8_t (* set_pagedir_phys)(karch_pagedir_t, karch_vaddr_t, karch_paddr_t, uint32_t flags);

    /* set the physical address to page table entry. */
    uint8_t (* set_pagetbl_phys)(karch_pagetbl_t, karch_vaddr_t, karch_paddr_t, uint32_t flags);

    /* get the page table from page directory entry. */
    karch_pagetbl_t (* get_pagetbl)(karch_pagedir_t, karch_vaddr_t);

    /* get the physical address from page directory entry. */
    karch_paddr_t (* get_pagedir_phys)(karch_pagedir_t, karch_vaddr_t);

    /* get the physical address from page table entry. */
    karch_paddr_t (* get_pagetbl_phys)(karch_pagetbl_t, karch_vaddr_t);

    /* translate `vaddr` to `paddr`, returns non-zero if succeed. */
    uint8_t (* translate_vaddr)(karch_pagedir_t, karch_paging_trans_t* trans);

    /* translate `paddr` to `vaddr`, returns non-zero if succeed. */
    uint8_t (* translate_paddr)(karch_pagedir_t, karch_paging_trans_t* trans);

    /* set the current running page directory to CPU. */
    uint8_t (* set_current_pagedir)(karch_pagedir_t);

    /* get the current running page directory from CPU. */
    karch_pagedir_t (* get_current_pagedir)();
} karch_paging_t;

#ifdef __cplusplus
}
#endif
#endif