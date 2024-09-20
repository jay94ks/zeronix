#ifndef __KERNEL_ARCH_X86_INC_X86_K86_PAGING_H__
#define __KERNEL_ARCH_X86_INC_X86_K86_PAGING_H__

#include <zeronix/arch/paging.h>
#include <x86/types.h>
#ifdef __ARCH_X86_INTERNALS__
#include <x86/boot/bootinfo.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* page directory address for . */
#define PAGE_DIR_ADDR  0x6000

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize the `karch`.
 * --> implemented in `k86/src/paging/early.c`.
 */
void karch_paging_early_init(bootinfo_t* info);
#endif

/**
 * verify the page directory is on physical address or not.
 * returns non-zero if succeed.
 */
uint8_t karch_paging_verify_pagedir(karch_pagedir_t ptr);

/**
 * verify the page table has 4KB aligned or not.
 * returns non-zero if succeed.
 */
uint8_t karch_paging_verify_pagetbl(karch_pagetbl_t ptr);

/**
 * set the page table to cover specified virtual address.
 * if `dst` is not 4MB aligned and `tbl` is not 4KB aligned, this returns zero.
 * and if `flags` has `BIG_PAGE` bit, it'll be cleared.
 */
uint8_t karch_paging_set_pagetbl(karch_pagedir_t pagedir, karch_vaddr_t dst, karch_pagetbl_t tbl, uint32_t flags);

/**
 * set the physical address to page directory entry.
 * if `dst` and `src` are not 4MB aligned, this returns zero.
 */
uint8_t karch_paging_set_pagedir_phys(karch_pagedir_t pagedir, karch_vaddr_t dst, karch_paddr_t src, uint32_t flags);

/**
 * set the physical address to page directory entry.
 * if `dst` and `src` are not 4KB aligned, this returns zero.
 */
uint8_t karch_paging_set_pagetbl_phys(karch_pagetbl_t pagetbl, karch_vaddr_t dst, karch_paddr_t src, uint32_t flags);

/**
 * get the page table from specified virtual address.
 * if the target entry does not point page table, returns null.
 */
karch_pagetbl_t karch_paging_get_pagetbl(karch_pagedir_t pagedir, karch_vaddr_t vaddr);

/**
 * get the physical base from specified virtual address.
 * if the target entry is not big page, returns null.
 */
karch_paddr_t karch_paging_get_pagedir_phys(karch_pagedir_t pagedir, karch_vaddr_t vaddr);

/**
 * get the physical base from specified virtual address.
 * this always success, and this looks just `vaddr`s 4K paging address.
 */
karch_paddr_t karch_paging_get_pagetbl_phys(karch_pagetbl_t pagetbl, karch_vaddr_t vaddr);

/**
 * translate `vaddr` to `paddr`, 
 * returns non-zero if succeed.
 */
uint8_t karch_paging_translate_vaddr(karch_pagedir_t pagedir, karch_paging_trans_t* trans);

/**
 * translate `paddr` to `vaddr.
 * returns non-zero if succeed.
 */
uint8_t karch_paging_translate_paddr(karch_pagedir_t pagedir, karch_paging_trans_t* trans);

/**
 * set the current page directory to the CPU.
 * warn: the pointer must be in physical space.
 */
uint8_t karch_paging_set_current(karch_pagedir_t pagedir);

/**
 * get the current page directory from the CPU.
 * warn: returned pointer is not always accessible.
 */
karch_pagedir_t karch_paging_get_current();

/* page size. */
#define I686_PAGE_SIZE		4096
#define I686_BIG_PAGE_SIZE	(I686_PAGE_SIZE*I686_VM_PT_ENTRIES)
#define I686_PAGE_DIR_ALIGN 4096

/* Page table specific flags. */
#define I686_VM_DIRTY    (1L<< 6)	/* Dirty */
#define I686_VM_PS  	 (1L<< 7)	/* Page size. */
#define I686_VM_GLOBAL   (1L<< 8)	/* Global. */
#define I686_VM_PTAVAIL1 (1L<< 9)	/* Available for use. */
#define I686_VM_PTAVAIL2 (1L<<10)	/* Available for use. */
#define I686_VM_PTAVAIL3 (1L<<11)	/* Available for use. */

#define I686_VM_PT_ENT_SIZE	    4	    /* Size of a page table entry */
#define I686_VM_DIR_ENTRIES	    1024	/* Number of entries in a page dir */
#define I686_VM_DIR_ENT_SHIFT	22	    /* Shift to get entry in page dir. */
#define I686_VM_PT_ENT_SHIFT	12	    /* Shift to get entry in page table */
#define I686_VM_PT_ENT_MASK	    0x3FF	/* Mask to get entry in page table */
#define I686_VM_PT_ENTRIES	    1024	/* Number of entries in a page table */
#define I686_VM_PFA_SHIFT	    22	    /* Page frame address shift */

/* Page directory specific flags. */
#define I686_VM_BIGPAGE	0x080	/* 4MB page */

/* I686 paging constants */
#define I686_VM_PRESENT	        0x001	/* Page is present */
#define I686_VM_WRITE	        0x002	/* Read/write access allowed */
#define I686_VM_READ	        0x000	/* Read access only */
#define I686_VM_USER	        0x004	/* User access allowed */
#define I686_VM_PWT	            0x008	/* Write through */
#define I686_VM_PCD	            0x010	/* Cache disable */
#define I686_VM_ACC	            0x020	/* Accessed */
#define I686_VM_ADDR_MASK_4MB   0xFFC00000 /* physical address */
#define I686_VM_OFFSET_MASK_4MB 0x003FFFFF /* physical address */

#define I686_VM_ADDR_MASK_4KB   0xFFFFF000
#define I686_VM_OFFSET_MASK_4KB 0x00000FFF

/* I686 paging 'functions' */
#define I686_VM_PTE(v)	(((v) >> I686_VM_PT_ENT_SHIFT) & I686_VM_PT_ENT_MASK)
#define I686_VM_PDE(v)	( (v) >> I686_VM_DIR_ENT_SHIFT)
#define I686_VM_PFA(e)	( (e) & I686_VM_ADDR_MASK)
#define I686_VM_PAGE(v)	( (v) >> I686_VM_PFA_SHIFT)

/* I686 pagefault error code bits */
#define I686_VM_PFE_P	0x01	
/* Pagefault caused by non-present page.
 * (otherwise protection violation.)
 */

#define I686_VM_PFE_W	0x02	/* Caused by write (otherwise read) */
#define I686_VM_PFE_U	0x04	/* CPU in user mode (otherwise supervisor) */

// --
#define I686_PAGE_DIR_SIZE (I686_VM_DIR_ENTRIES * sizeof(uint32_t))
#define I686_PAGE_TBL_SIZE (I686_VM_PT_ENTRIES * sizeof(uint32_t))

#define MASK_ADDR_4MB(x) (((uint32_t)(x)) & I686_VM_ADDR_MASK_4MB)
#define MASK_ADDR_OFFSET_4MB(x) (((uint32_t)(x)) & I686_VM_OFFSET_MASK_4MB)

// --
#define MASK_ADDR_4KB(x) (((uint32_t)(x)) & I686_VM_ADDR_MASK_4KB)
#define MASK_ADDR_OFFSET_4KB(x) (((uint32_t)(x)) & I686_VM_OFFSET_MASK_4KB)

#ifdef __cplusplus
}
#endif
#endif