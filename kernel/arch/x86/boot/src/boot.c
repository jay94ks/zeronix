#include <zeronix/types.h>
#include <x86/boot/multiboot.h>
#include <x86/boot/bootinfo.h>
#include <x86/boot/bootenv.h>
#include <x86/k86/paging.h>
#include "tpg.h"

// --
static uint32_t pagedir[I686_VM_DIR_ENTRIES] __aligned(4096);
bootinfo_t temp_bootinfo;

// --
void kboot_panic(const char* text);
void kboot_read_mbinfo(bootinfo_t* boot, mbinfo_t* info);
void kboot_add_mmap(bootinfo_t* boot, mb_mmap_t* mmap);
void kboot_tpg_identity(bootinfo_t* info);
void kboot_tpg_map_kernel(bootinfo_t* info);
void kboot_tpg_enable_paging();

// --> 4K address mask. because i686 manages address space in 4k unit.
#define KBOOT_i686_SHIFT_4K     12
#define KBOOT_i686_MASK_4K      0x00000fffu
#define KBOOT_i686_ADDR_MAX     (~KBOOT_i686_MASK_4K)

// --
#define KBOOT_VIDEO ((uint16_t*) 0xb8000)
#define MASK_ADDR_4MB(x) ((x) & I686_VM_ADDR_MASK_4MB)

enum {
    TPG_CPUF_PAE = 1,   // --> PAE support.
    TPG_CPUF_PGE = 2,   // --> PGE support.
};

/**
 * initialize the kernel's boot stage.
 * this make temporary page mappings to jump to `karch_init`.
 */
bootinfo_t* kboot(uint32_t magic, uint32_t ebx) {
    if (magic != MULTIBOOT_LOADER_MAGIC) {
        kboot_panic("panic: multiboot magic mismatch.\n");
    }

    bootinfo_t* boot = &temp_bootinfo;

    kboot_read_mbinfo(boot, (mbinfo_t*) ebx);
    kboot_tpg_identity(boot);

    kboot_tpg_map_kernel(boot);
    tpg_write_cr3((uint32_t) pagedir);

    kboot_tpg_enable_paging();
    return boot;
}

void kboot_panic(const char* text) {
    uint16_t* vga = KBOOT_VIDEO;
    while (text && *text) {
        *vga++ = *text | (15 << 8);
        text++;
    }

    // --> hold here.
    while(1);
}

void kboot_read_mbinfo(bootinfo_t* boot, mbinfo_t* info) {
    boot->mem_high_phys = 0;

    if ((info->flags & MBINFO_CMDLINE) != 0) {
        // TODO: parse command line.
    }

    boot->mmap_n = 0;

    // --> put all memory map here.
    if ((info->flags & MBINFO_MEM_MAP) != 0) {
        mb_mmap_t* mmap = (mb_mmap_t*) info->mmap_addr;

        while((uint32_t)mmap < info->mmap_addr + info->mmap_len) {
            kboot_add_mmap(boot, mmap);
            mmap = (mb_mmap_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
        }
    }
    else {
        mb_mmap_t temp;

        temp.type = MBMMAP_AVAIL;
        temp.addr = 0;
        temp.len = info->mem_low * 1024;

        // --> lower 1MB.
        kboot_add_mmap(boot, &temp);
        
        temp.addr = 0x100000;
        temp.len = info->mem_high * 1024;

        // --> higher than 1MB.
        kboot_add_mmap(boot, &temp);
    }
}

void kboot_add_mmap(bootinfo_t* boot, mb_mmap_t* mmap) {
    uint32_t addr = mmap->addr;
    uint32_t end_addr = addr + mmap->len;
    
    // --> truncate over 4GB, currently, can not handle it.
    if (addr > KBOOT_i686_ADDR_MAX) {
        addr = KBOOT_i686_ADDR_MAX;
    }

    if (end_addr > KBOOT_i686_ADDR_MAX) {
        end_addr = KBOOT_i686_ADDR_MAX;
    }

    // --> round up.
    //   : equalivant: (addr % 4096) != 0 --> addr = (addr / 4096 + 1) * 4096.
    if ((addr & KBOOT_i686_MASK_4K) != 0) {
        addr = ((addr >> KBOOT_i686_SHIFT_4K) + 1) << KBOOT_i686_SHIFT_4K;
    }

    // --> round down.
    //   : equalivant: end_addr = (end_addr / 4096) * 4096.
    end_addr = (end_addr >> 12) << 12;
    if (addr >= end_addr) {
        return;
    }

    uint32_t n = boot->mmap_n++;
    if (n >= BOOTINFO_MAX_MEMMAP) {
        kboot_panic("panic: no mmap slot available.\n");
    }

    boot->mmap[n].addr = addr;
    boot->mmap[n].len = end_addr - addr;
    boot->mmap[n].type = mmap->type;

    // --> remember high phys address to get kernel memory.
    if (mmap->type == MBMMAP_AVAIL) {
        if (end_addr > boot->mem_high_phys) {
            boot->mem_high_phys = end_addr;
        }
    }
}

void kboot_tpg_identity(bootinfo_t* info) {
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

        pagedir[i] = phys | flags;
    }
}

void kboot_tpg_map_kernel(bootinfo_t* info) {
    uint32_t mapped = 0, phys = mb_phys_base;
    uint32_t pde = mb_virt_base / I686_BIG_PAGE_SIZE;

    while (mapped < mb_size) {
        pagedir[pde]
            = phys | I686_VM_PRESENT
            | I686_VM_BIGPAGE | I686_VM_WRITE
            ;

        mapped += I686_BIG_PAGE_SIZE;
        phys += I686_BIG_PAGE_SIZE;
        pde++;
    }
}

uint32_t kboot_tpg_check_cpuf() {
    uint32_t eax = 0, ebx, edx, ecx;
    tpg_cpuid(&eax, &ebx, &edx, &ecx);

    if (eax <= 0) {
        return 0; // --> <= below than pentium.
    }

    eax = 1;
    tpg_cpuid(&eax, &ebx, &edx, &ecx);

    // eax: stepping (0 ... 3), model (4 ... 7), familly (8 ... 11)
    uint32_t retval = 0;
    if ((edx & CPUID1_EDX_PGE) != 0) {
        // --> PGE supported.
        retval |= TPG_CPUF_PGE;
    }

    if ((edx & CPUID1_EDX_PAE) != 0) {
        // --> PAE supported.
        retval |= TPG_CPUF_PAE;
    }

    return retval;
}

void kboot_tpg_enable_paging() {
    uint32_t cpuf = kboot_tpg_check_cpuf();
    uint32_t cr0, cr4;
    
    cr0 = tpg_read_cr0();
    cr4 = tpg_read_cr4();

#ifdef __ZK_ENABLE_PAE
    if ((cpuf & TPG_CPUF_PAE) != 0) {
        kboot_panic("panic: the CPU has no PAE support.\n");
    }
#endif

    /* clear PG, PGE and PSE. */
    tpg_write_cr0(cr0 & ~I686_CR0_PG);
    tpg_write_cr4(cr4 & ~(I686_CR4_PGE | I686_CR4_PSE));

    cr0 = tpg_read_cr0();
    cr4 = tpg_read_cr4();

    cr4 |= I686_CR4_PSE;
    tpg_write_cr4(cr4);

    cr0 |= I686_CR0_PG;
    tpg_write_cr0(cr0);

    cr0 |= I686_CR0_WP;
    tpg_write_cr0(cr0);

    if (cpuf & TPG_CPUF_PGE) {
        cr4 |= I686_CR4_PGE;
    }

    tpg_write_cr4(cr4);
}