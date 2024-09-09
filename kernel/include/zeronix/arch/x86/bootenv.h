#ifndef __ARCH_I686_MULTIBOOT_BOOTENV_H__
#define __ARCH_I686_MULTIBOOT_BOOTENV_H__

#include <zeronix/types.h>

/* defined from linker script. */
#ifdef __cplusplus
extern "C" {
#endif
extern void* _mb_phys_base;
extern void* _mb_virt_base;
extern void* _mb_addr_offset;
extern void* _mb_unpaged_start;
extern void* _mb_unpaged_end;
extern void* _mb_start;
extern void* _mb_end;
extern void* _mb_size;
#ifdef __cplusplus
}
#endif

/* translate to correct value. */
#define mb_phys_base        (uint32_t)(&_mb_phys_base)
#define mb_virt_base        (uint32_t)(&_mb_virt_base)
#define mb_addr_offset      (uint32_t)(&_mb_addr_offset)
#define mb_unpaged_start    (uint32_t)(&_mb_unpaged_start)
#define mb_unpaged_end      (uint32_t)(&_mb_unpaged_end)
#define mb_start            (uint32_t)(&_mb_start)
#define mb_end              (uint32_t)(&_mb_end)
#define mb_size             (uint32_t)(&_mb_size)

#endif