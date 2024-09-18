#define __ARCH_X86_INTERNALS__ 1
#include <x86/k86/mmap.h>
#include <x86/k86/paging.h>
#include <x86/boot/bootenv.h>
#include <x86/i686.h>

#include <zeronix/arch/mmap.h>
#include <zeronix/kstring.h>

// --
#define PAGE_DIR_ADDR_END   (PAGE_DIR_ADDR + sizeof(uint32_t) * I686_VM_DIR_ENTRIES)

// --
karch_mmap_t mmap_free;          // --> free, avail phys.
karch_mmap_t mmap_kern_virt;     // --> reserved, unavail virts, kernel space.

// --
uint8_t karch_mmap_add(karch_mmap_t* mmap, uint32_t addr, uint32_t len);
uint8_t karch_mmap_subtract(karch_mmap_t* mmap, uint32_t addr, uint32_t len);

// --
void karch_mmap_init(bootinfo_t* info) {
    kmemset(&mmap_free, 0, sizeof(mmap_free));
    kmemset(&mmap_kern_virt, 0, sizeof(mmap_kern_virt));

    // --> add `free` regions first.
    for (uint32_t i = 0; i < info->mmap_n; ++i) {
        bootinfo_mmap_t* mem = &info->mmap[i];

        if (mem->type == BOOTMMAP_AVAIL) {
            karch_mmap_add(&mmap_free, mem->addr, mem->len);
        }
    }

    // --> subtract old bios area here.
    karch_mmap_subtract(&mmap_free, BIOS_OLD_BDA_START, BIOS_OLD_BDA_END - BIOS_OLD_BDA_START);
    karch_mmap_subtract(&mmap_free, BIOS_EBDA_START, BIOS_EBDA_END - BIOS_EBDA_START);
    karch_mmap_subtract(&mmap_free, BIOS_VIDEO_START, BIOS_VIDEO_END - BIOS_VIDEO_START);
    karch_mmap_subtract(&mmap_free, BIOS_EXPAN_START, BIOS_EXPAN_END - BIOS_EXPAN_START);
    karch_mmap_subtract(&mmap_free, BIOS_BOARD_START, BIOS_BOARD_END - BIOS_BOARD_START);

    // --> subtract page directory.
    karch_mmap_subtract(&mmap_free, PAGE_DIR_ADDR, PAGE_DIR_ADDR_END - PAGE_DIR_ADDR);

    // --> add reserved kernel spaces.
    karch_mmap_add(&mmap_kern_virt, BIOS_OLD_BDA_START, BIOS_OLD_BDA_END - BIOS_OLD_BDA_START);
    karch_mmap_add(&mmap_kern_virt, BIOS_EBDA_START, BIOS_EBDA_END - BIOS_EBDA_START);
    karch_mmap_add(&mmap_kern_virt, BIOS_VIDEO_START, BIOS_VIDEO_END - BIOS_VIDEO_START);
    karch_mmap_add(&mmap_kern_virt, BIOS_EXPAN_START, BIOS_EXPAN_END - BIOS_EXPAN_START);
    karch_mmap_add(&mmap_kern_virt, BIOS_BOARD_START, BIOS_BOARD_END - BIOS_BOARD_START);
    
    // --> add page directory to kernel space.
    karch_mmap_add(&mmap_kern_virt, PAGE_DIR_ADDR, PAGE_DIR_ADDR_END - PAGE_DIR_ADDR);

    // --> then exclude all unusable regions.
    for (uint32_t i = 0; i < info->mmap_n; ++i) {
        bootinfo_mmap_t* mem = &info->mmap[i];

        if (mem->type != BOOTMMAP_AVAIL) {
            karch_mmap_subtract(&mmap_free, mem->addr, mem->len);
            karch_mmap_add(&mmap_kern_virt, mem->addr, mem->len);
        }
    }

    // --> finally, subtract kernel regions.
    // 1. kernel's virtual address.
    // 2. kernel's physical space.
    karch_mmap_add(&mmap_kern_virt, mb_start, mb_size);
    karch_mmap_subtract(&mmap_free, 
        (mb_start - mb_addr_offset) + mb_phys_base, 
        mb_size);
}

/**
 * insert an entity to memory map.
 */
uint8_t karch_mmap_insert(karch_mmap_t* mmap, uint32_t i, uint32_t addr, uint32_t len) {
    if (mmap->count >= MMAP_MAX) {
        return 0; // --> failed to add memory map.
    }

    if (i >= mmap->count) {
        i = mmap->count++;
        mmap->mmap[i].addr = addr;
        mmap->mmap[i].len = len;
        mmap->total += len;
        return 1;
    }

    // --> push all elements back.
    for (uint32_t j = mmap->count; j > i; j--) {
        mmap->mmap[j] = mmap->mmap[j - 1];
    }
    
    mmap->count++;
    mmap->mmap[i].addr = addr;
    mmap->mmap[i].len = len;
    mmap->total += len;

    return 1;
}

/**
 * remove an entity at specified index.
 */
uint8_t karch_mmap_remove_at(karch_mmap_t* mmap, uint32_t i) {
    if (mmap->count <= i) {
        return 0;
    }

    for (uint32_t j = i; j < mmap->count + 1; ++j) {
        mmap->mmap[j] = mmap->mmap[j + 1];
    }

    mmap->count--;
    return 1;
}

/**
 * add specified memory region to memory map.
 */
uint8_t karch_mmap_add(karch_mmap_t* mmap, uint32_t addr, uint32_t len) {
    uint32_t addr_end = addr + len;

    for (uint32_t i = 0; i < mmap->count; ++i) {
        karch_mmap_elem_t* now = &mmap->mmap[i];
        uint32_t now_end = now->addr + now->len;

        // --> merge if linear.
        if (now->addr == addr_end) {
            now->addr = addr;
            now->len += len;
            return 1;
        }

        // --> merge if linear.
        if (now_end == addr) {
            now->len += len;
            return 1;
        }

        // --> now include specified partial region.
        if (now->addr < addr && now_end > addr) {
            if (addr_end <= now_end) {
                // --> completely contained.
                return 1;
            }

            uint32_t over = addr_end - now_end;
            now->len += over;
            return 1;
        }

        // --> found highest than specified.
        if (now->addr > addr_end) {
            return karch_mmap_insert(mmap, i, addr, len);
        }
    }

    // --> insert at end of mmap.
    return karch_mmap_insert(mmap, mmap->count, addr, len);
}

/**
 * subtract specified memory region from memory map.
 */
uint8_t karch_mmap_subtract(karch_mmap_t* mmap, uint32_t addr, uint32_t len) {
    uint8_t ret = 0;

    while (mmap->count > 0 && len) {
        uint8_t again = 0;

        for (uint32_t i = 0; i < mmap->count; ++i) {
            if (!len) {
                break;
            }

            karch_mmap_elem_t* now = &(mmap->mmap[i]);
            uint32_t now_end = now->addr + now->len;

            // --> skip unnecessary region fast.
            if (now->addr > addr || now_end < addr) {
                continue;
            }

            // --> exactly same start address.
            if (now->addr == addr) {
                uint32_t slice 
                    = now->len > len 
                    ? len : now->len;

                now->addr += slice;
                now->len -= slice;

                addr += slice;
                len -= slice;

                // --> remove the region.
                if (now->len <= 0) {
                    karch_mmap_remove_at(mmap, i);
                }
                
                again = 1;
                break;
            }

            // --> split this region.
            //   : then try again to subtract.
            // 1. now->addr ~ addr  : front.
            // 2. addr ~ now_end    : back.

            uint32_t s1_addr = now->addr;
            uint32_t s1_len = addr - now->addr;
            uint32_t s2_len = now_end - addr;

            if (s2_len) {
                karch_mmap_insert(mmap, i + 1, addr, s2_len);
            }

            if (s1_len) {
                now->addr = s1_addr;
                now->len = s1_len;
            }
        }


        if (!again) {
            break;
        }
    }

    return ret;
}