#include <zeronix/arch/paging.h>
#include <x86/k86/paging.h>
#include <x86/k86/mmap.h>
#include <x86/klib.h>

// --

/**
 * get the physical page directory address.
 */
karch_paddr_t karch_paging_get_phy_pagedir(void* ptr, size_t size) {
    const uint32_t cr3 = read_cr3();

    // --> read page directory.
    karch_pagedir_t dir = (karch_pagedir_t) cr3;
    karch_paging_trans_t trans;

    trans.vaddr = AS_VIRT(ptr);
    trans.vlen = size;

    // --> not mapped, so couldn't look-up.
    if (!karch_paging_translate_vaddr(dir, &trans)) {
        return 0;
    }

    // --> not linear, so it can not be translated.
    if (trans.plen != trans.vlen) {
        return 0;
    }

    // --> returns physical address.
    return trans.paddr;
}

uint8_t karch_paging_verify_pagedir(karch_pagedir_t ptr) {
    if (!ptr) {
        return 0;
    }

    // --> translate the directory to physical address.
    karch_paddr_t phys = karch_paging_get_phy_pagedir(ptr, I686_PAGE_DIR_SIZE);
    if (!phys) {
        return 0;
    }

    return 1;
}

uint8_t karch_paging_verify_pagetbl(karch_pagetbl_t ptr) {
    if (!ptr) {
        return 0;
    }

    // --> translate the directory to physical address.
    karch_paddr_t phys = karch_paging_get_phy_pagedir(ptr, I686_PAGE_TBL_SIZE);
    if (!phys) {
        return 0;
    }

    // --> the page directory is not 4KB aligned.
    if (MASK_ADDR_OFFSET_4KB(phys) != 0) {
        return 0;
    }

    return 1;
}

uint8_t karch_paging_set_pagetbl(karch_pagedir_t pagedir, karch_vaddr_t dst, karch_pagetbl_t tbl, uint32_t flags) {
    if (MASK_ADDR_OFFSET_4MB(dst) != 0) {
        return 0;
    }

    // --> alignment error.
    if (MASK_ADDR_OFFSET_4KB(tbl) != 0) {
        return 0;
    }

    const uint32_t index = MASK_ADDR_4MB(dst) >> I686_VM_PT_ENT_SHIFT;
    uint32_t* entries = (uint32_t*) pagedir;

    // --> make a new entry.
    uint32_t new_de 
        = MASK_ADDR_4KB(entries[index])
        | MASK_ADDR_OFFSET_4KB(flags & (~I686_VM_BIGPAGE));

    // --> then put it to directory.
    entries[index] = new_de;
    return 1;
}

uint8_t karch_paging_set_pagedir_phys(karch_pagedir_t pagedir,
    karch_vaddr_t dst, karch_paddr_t src, uint32_t flags) 
{
    if (MASK_ADDR_OFFSET_4MB(dst) != 0) {
        return 0;
    }

    if (MASK_ADDR_OFFSET_4MB(src) != 0) {
        return 0;
    }

    const uint32_t index = MASK_ADDR_4MB(dst) >> I686_VM_DIR_ENT_SHIFT;
    uint32_t* entries = (uint32_t*) pagedir;

    // --> make a new entry.
    uint32_t new_de
        = MASK_ADDR_4MB(src)
        | MASK_ADDR_OFFSET_4MB(flags | I686_VM_BIGPAGE);

    entries[index] = new_de;
    return 1;
}

uint8_t karch_paging_set_pagetbl_phys(karch_pagetbl_t pagetbl,
    karch_vaddr_t dst, karch_paddr_t src, uint32_t flags)
{
    if (MASK_ADDR_OFFSET_4KB(dst) != 0) {
        return 0;
    }

    if (MASK_ADDR_OFFSET_4KB(src) != 0) {
        return 0;
    }

    const uint32_t index = MASK_ADDR_4KB(dst) >> I686_VM_PT_ENT_SHIFT;
    uint32_t* entries = (uint32_t*) pagetbl;

    // --> make a new entry.
    uint32_t new_de
        = MASK_ADDR_4KB(src)
        | MASK_ADDR_OFFSET_4KB(flags & (~I686_VM_BIGPAGE));

    entries[index] = new_de;
    return 1;
}

karch_pagetbl_t karch_paging_get_pagetbl(karch_pagedir_t pagedir, karch_vaddr_t vaddr) {
    const uint32_t index = MASK_ADDR_4MB(vaddr) >> I686_VM_DIR_ENT_SHIFT;
    const uint32_t* entries = (uint32_t*) pagedir;

    if ((entries[index] & I686_VM_BIGPAGE) != 0) {
        return 0; // --> not a page table.
    }

    return (karch_pagetbl_t) MASK_ADDR_4KB(entries[index]);
}

karch_paddr_t karch_paging_get_pagedir_phys(karch_pagedir_t pagedir, karch_vaddr_t vaddr) {
    const uint32_t index = MASK_ADDR_4MB(vaddr) >> I686_VM_DIR_ENT_SHIFT;
    const uint32_t* entries = (uint32_t*) pagedir;

    if ((entries[index] & I686_VM_BIGPAGE) == 0) {
        return 0; // --> not a big page.
    }

    return AS_PHYS(MASK_ADDR_4MB(entries[index]));
}

karch_paddr_t karch_paging_get_pagetbl_phys(karch_pagetbl_t pagetbl, karch_vaddr_t vaddr) {
    vaddr = AS_VIRT(MASK_ADDR_OFFSET_4MB(vaddr)); // --> to page table local address.
    const uint32_t index = MASK_ADDR_4KB(vaddr) >> I686_VM_PT_ENT_SHIFT;

    return MASK_ADDR_4KB(((uint32_t*) pagetbl)[index]);
}

uint8_t karch_paging_translate_vaddr(karch_pagedir_t pagedir, karch_paging_trans_t* trans) {
    if (!trans) {
        return 0;
    }

    uint32_t index = ((uint32_t)trans->vaddr) >> I686_VM_DIR_ENT_SHIFT;
    uint32_t entry = ((uint32_t*)pagedir)[index];


    // --> not present.
    if ((entry & I686_VM_PRESENT) == 0) {
        return 0;
    }
    
    uint32_t vbase = MASK_ADDR_4MB(trans->vaddr);
    uint32_t voff = MASK_ADDR_OFFSET_4MB(trans->vaddr);

    // --> if it is big-page:
    if ((entry & I686_VM_BIGPAGE) != 0) {
        // --> compute based on 4MB in page directory scope.
        uint32_t vend = trans->vlen + voff;
        uint32_t pbase = MASK_ADDR_4MB(entry);
        
        // --> compute physical address.
        trans->paddr = AS_PHYS(pbase + voff);
        trans->plen = trans->vlen;

        // --> and if the end of vaddr is larger than big page size:
        if (vend > I686_BIG_PAGE_SIZE) {
            trans->plen = I686_BIG_PAGE_SIZE - voff;
        }
    }

    // --> page table look-up is required.
    else {
        // --> compute based on 4KB in page table scope.
        uint32_t vend = trans->vlen + voff;

        // --> fetch table entry and compute physical base address.
        uint32_t* pagetbl = (uint32_t*)(entry & I686_VM_ADDR_MASK_4KB);
        const uint32_t pt_entry = pagetbl[voff >> I686_VM_PT_ENT_SHIFT];

        // --> not present.
        if ((pt_entry & I686_VM_PRESENT) == 0) {
            return 0;
        }

        uint32_t pbase = pt_entry & I686_VM_ADDR_MASK_4KB;
        voff = MASK_ADDR_OFFSET_4KB(voff);

        // --> compute physical address.
        trans->paddr = AS_PHYS(pbase + voff);
        trans->plen = trans->vlen;

        // --> and if the end of vaddr is larger than page size:
        if (vend > I686_PAGE_SIZE) {
            trans->plen = I686_PAGE_SIZE - voff;
        }
    }

    return 1;
}

uint8_t karch_paging_translate_paddr(karch_pagedir_t pagedir, karch_paging_trans_t* trans) {
    if (!trans) {
        return 0;
    }

    const uint32_t* entries = (uint32_t*) pagedir;
    uint32_t masked_4mb = MASK_ADDR_4MB(trans->paddr);
    uint32_t masked_4kb = MASK_ADDR_4KB(trans->paddr);

    uint32_t page_size = 0; // --> page size.
    uint32_t phys_off = 0;  // --> physical offset.

    for (uint32_t i = 0; i < I686_VM_DIR_ENTRIES; ++i) {
        const uint32_t dir_ent = entries[i];
        const uint32_t dir_base = i * I686_BIG_PAGE_SIZE;

        if ((dir_ent & I686_VM_PRESENT) == 0) {
            // --> not present.
            continue;
        }

        // --> it mapped on big page.
        if ((dir_ent & I686_VM_BIGPAGE) != 0) {
            if (MASK_ADDR_4MB(dir_ent) != masked_4mb) {
                continue;
            }
            
            page_size = I686_BIG_PAGE_SIZE;
            phys_off = MASK_ADDR_OFFSET_4MB(trans->paddr);
            trans->vaddr = AS_PHYS(dir_base + phys_off);
            break;
        }

        // --> check page table entries.
        const uint32_t* pt_entries = (uint32_t*)(dir_ent & I686_VM_ADDR_MASK_4KB);
        uint8_t found = 0;

        for(uint32_t j = 0; j < I686_VM_PT_ENTRIES; ++j) {
            const uint32_t pt_ent = pt_entries[j];
            
            if ((pt_ent & I686_VM_PRESENT) == 0) {
                // --> not present.
                continue;
            }

            if ((pt_ent & I686_VM_ADDR_MASK_4KB) != masked_4kb) { 
                // --> physical base mismatch.
                continue;
            }

            found = 1;
            page_size = I686_PAGE_SIZE;
            phys_off = MASK_ADDR_OFFSET_4KB(trans->paddr);
            trans->vaddr = AS_PHYS(dir_base + j * I686_PAGE_SIZE + phys_off);
            break;
        }

        if (found) {
            break;
        }
    }

    // --> not mapped.
    if (!page_size) {
        return 0;
    }

    // --> copy length first, and modify it.
    trans->vlen = trans->plen;

    // --> if the physical length is larger than page size, trim it.
    uint32_t pend = phys_off + trans->plen;
    if (pend > page_size) {
        trans->vlen = page_size - phys_off;
    }

    return 1;
}

uint8_t karch_paging_set_current(karch_pagedir_t pagedir) {
    if (!pagedir) {
        return 0;
    }

    /*
    karch_paddr_t phys = karch_paging_get_phy_pagedir(pagedir, I686_PAGE_DIR_SIZE);
    if (!phys) {
        return 0;
    }*/

    // --> the page directory is not 4MB aligned.
    if (MASK_ADDR_OFFSET_4MB(pagedir) != 0) {
        return 0;
    }

    write_cr3((uint32_t) pagedir);
    return 1;
}

karch_pagedir_t karch_paging_get_current() {
    return (karch_pagedir_t) read_cr3();
}