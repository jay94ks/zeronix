#include "page.h"
#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/bootenv.h>
#include <zeronix/arch/x86/i686.h>

// --
#define MASK_ADDR_4MB(x) ((x) & I686_VM_ADDR_MASK_4MB)

// --
void karch_page_identity(kbootinfo_t* info);
void karch_page_remap_kernel(kbootinfo_t* info);
void karch_page_reenable();

// --
void karch_init_page(kbootinfo_t* info) {
    karch_page_identity(info);
    karch_page_remap_kernel(info);

    write_cr3((uint32_t) info->pagedir);
}

void karch_page_identity(kbootinfo_t* info) {
    /* identity mapping. */
    for(uint32_t i = 0; i < I686_VM_DIR_ENTRIES; ++i) {
        uint32_t flags 
            = I686_VM_PRESENT | I686_VM_BIGPAGE
            | I686_VM_USER | I686_VM_WRITE
            ;

        uint32_t phys = i * I686_BIG_PAGE_SIZE;

        if (MASK_ADDR_4MB(info->mem_high_phys) <= MASK_ADDR_4MB(phys)) {
            flags |= I686_VM_PWT | I686_VM_PCD;
        }

        info->pagedir[i] = phys | flags;
    }
}

void karch_page_remap_kernel(kbootinfo_t* info) {
    uint32_t mapped = 0, phys = mb_phys_base;
    uint32_t pde = mb_virt_base / I686_BIG_PAGE_SIZE;

    while (mapped < mb_size) {
        info->pagedir[pde]
            = phys | I686_VM_PRESENT
            | I686_VM_BIGPAGE | I686_VM_WRITE
            ;

        mapped += I686_BIG_PAGE_SIZE;
        phys += I686_BIG_PAGE_SIZE;
        pde++;
    }

    info->freepde = pde;
}
