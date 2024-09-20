#define __ARCH_X86_INTERNALS__ 1
#include <zeronix/arch/arch.h>
#include <x86/interface.h>
#include <x86/k86/mmap.h>
#include <x86/k86/paging.h>
#include <x86/k86/systick.h>
#include <x86/k86/irq.h>

karch_irq_frame_t* karch_irq_get_frame_cv() {
    return (karch_irq_frame_t*) karch_irq_get_frame();
}

/**
 * exported to <zeronix/arch/arch.h>.
 */
void karch_interface(karch_t* arch) {
    if (!arch) {
        return;
    }

    arch->mem_free = &mmap_free;
    arch->mem_kern_virt = &mmap_kern_virt;
    arch->systick_freq = systick_freq;
    karch_interface_smp(&arch->smp);
    
    // --> set IRQ interfaces.
    arch->irq.count = karch_irq_count();
    arch->irq.reg = karch_irq_register;
    arch->irq.unreg = karch_irq_unregister;
    arch->irq.mask = karch_irq_mask;
    arch->irq.unmask = karch_irq_unmask;
    arch->irq.get_frame = karch_irq_get_frame_cv;

    // --> set paging interfaces.
    arch->paging.page_size = I686_PAGE_SIZE;
    arch->paging.page_align = I686_PAGE_SIZE;
    arch->paging.big_page_size = I686_BIG_PAGE_SIZE;
    arch->paging.big_page_align = I686_BIG_PAGE_SIZE;
    arch->paging.page_directory_size = I686_PAGE_DIR_SIZE;
    arch->paging.page_table_size = I686_PAGE_TBL_SIZE;
    
    arch->paging.verify_pagedir = karch_paging_verify_pagedir;
    arch->paging.verify_pagetbl = karch_paging_verify_pagetbl;
    arch->paging.set_pagetbl = karch_paging_set_pagetbl;
    arch->paging.set_pagedir_phys = karch_paging_set_pagedir_phys;
    arch->paging.set_pagetbl_phys = karch_paging_set_pagetbl_phys;
    arch->paging.get_pagetbl = karch_paging_get_pagetbl;
    arch->paging.get_pagedir_phys = karch_paging_get_pagedir_phys;
    arch->paging.get_pagetbl_phys = karch_paging_get_pagetbl_phys;
    arch->paging.translate_vaddr = karch_paging_translate_vaddr;
    arch->paging.translate_paddr = karch_paging_translate_paddr;
    arch->paging.set_current_pagedir = karch_paging_set_current;
    arch->paging.get_current_pagedir = karch_paging_get_current;
}