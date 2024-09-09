#ifndef __ZERONIX_ARCH_X86_MULTIBOOT_H__
#define __ZERONIX_ARCH_X86_MULTIBOOT_H__

#include <zeronix/types.h>

/* multiboot number types. */
typedef unsigned char       mb_uint8_t;
typedef unsigned short      mb_uint16_t;
typedef unsigned int        mb_uint32_t;
typedef unsigned long long  mb_uint64_t;

// --> multiboot header's magic number.
#define MULTIBOOT_HEADER_MAGIC  0x1badb002
#define MULTIBOOT_LOADER_MAGIC  0x2badb002

/**
 * mb_flag_t.
 */
typedef enum {
    MBFLAG_PAGE_ALIGN  = 0x00000001,
    MBFLAG_MEMORY_INFO = 0x00000002,
    MBFLAG_VIDEO_MODE  = 0x00000004,
    MBFLAG_AOUT_KLUDGE = 0x00010000
} mb_flag_t;

/**
 * mb_header_t.
 */
typedef struct {
    mb_uint32_t magic;      // --> MULTIBOOT_HEADER_MAGIC.
    mb_uint32_t flags;      // --> MBFLAG_PAGE_ALIGN | MBFLAG_MEMORY_INFO.
    mb_uint32_t checksum;   // --> -(magic + flags).

    /* valid if flags have `MBFLAG_AOUT_KLUDGE`. */
    mb_uint32_t header_addr;
    mb_uint32_t load_addr;
    mb_uint32_t load_end_addr;
    mb_uint32_t bss_end_addr;
    mb_uint32_t entry_addr;

    /* valid if flags have `MBFLAG_VIDEO_MODE`. */
    mb_uint32_t mode_type;
    mb_uint32_t width;
    mb_uint32_t height;
    mb_uint32_t depth;
} mb_header_t;

/**
 * mb_aout_symtbl_t.
*/
typedef struct {
    mb_uint32_t tabsize;
    mb_uint32_t strsize;
    mb_uint32_t addr;
    mb_uint32_t rsv;
} mb_aout_symtbl_t;

/**
 * mb_elf_shdr_t.
 */
typedef struct {
    mb_uint32_t num;
    mb_uint32_t size;
    mb_uint32_t addr;
    mb_uint32_t shndx;
} mb_elf_shdr_t;

/**
 * mbinfo_flag_t.
 */
typedef enum {
    MBINFO_MEMORY   = 0x0001,
    MBINFO_BOOTDEV  = 0x0002,
    MBINFO_CMDLINE  = 0x0004,
    MBINFO_MODULES  = 0x0008,
    MBINFO_AMOUTSYM = 0x0010,
    MBINFO_ELF_SHDR = 0x0020,
    MBINFO_MEM_MAP  = 0x0040,
    MBINFO_DRIVES   = 0x0080,
    MBINFO_CONF_TBL = 0x0100,
    MBINFO_LOADER   = 0x0200,
    MBINFO_APM_TBL  = 0x0400,
    MBINFO_VBE_INFO = 0x0800,
    MBINFO_FRAMEBUF = 0x1000,
} mbinfo_flag_t;

/**
 * mbinfo_t.
 */
typedef struct {
    mb_uint32_t flags;  // --> mbinfo_flag_t

    /* bios memory info. */
    mb_uint32_t mem_low;
    mb_uint32_t mem_high;

    /* "root" partition. */
    mb_uint32_t bootdev;

    /* kernel command line. */
    mb_uint32_t cmdline;

    /* boot modules. */
    mb_uint32_t mods_cnt;
    mb_uint32_t mods_addr;

    /* aout/elf info. */
    union {
        mb_aout_symtbl_t aout;
        mb_elf_shdr_t elfs;
    };

    /* memory map. */
    mb_uint32_t mmap_len;
    mb_uint32_t mmap_addr;

    /* drive info. */
    mb_uint32_t drives_len;
    mb_uint32_t drives_addr;

    /* config table. */
    mb_uint32_t conf_tbl;

    /* loader name. */
    mb_uint32_t loader_name;

    /* APM table. */
    mb_uint32_t apm_tbl;

    /* video. */
    mb_uint32_t vbe_ctl;
    mb_uint32_t vbe_mode_info;
    mb_uint16_t vbe_mode;
    mb_uint16_t vbe_intf_seg;
    mb_uint16_t vbe_intf_off;
    mb_uint16_t vbe_intf_len;

    /* frame buf. */
    mb_uint64_t fbuf_addr;
    mb_uint32_t fbuf_pitch;
    mb_uint32_t fbuf_width;
    mb_uint32_t fbuf_height;
    mb_uint8_t  fbuf_bpp;
    mb_uint8_t  fbuf_type; // --> mb_fbuf_type_t.

    union {
        struct {
            mb_uint32_t fbuf_paddr; // --> palette address.
            mb_uint16_t fbuf_pncol; // --> palette num colors.
        };

        struct {
            mb_uint8_t  fbuf_rfp;   // --> red field pos.
            mb_uint8_t  fbuf_rms;   // --> red mask size.
            mb_uint8_t  fbuf_gfp;   // --> green.
            mb_uint8_t  fbuf_gms;
            mb_uint8_t  fbuf_bfp;   // --> blue.
            mb_uint8_t  fbuf_bms;
        };
    };
} mbinfo_t;

/**
 * mb_fbuf_type_t.
 */
typedef enum {
    MBFBUF_INDEXED = 0,
    MBFBUF_RGB = 1,
    MBFBUF_EGA_TEXT = 2
} mb_fbuf_type_t;

/**
 * mb_color_t.
 */
typedef struct {
    mb_uint8_t  r;
    mb_uint8_t  g;
    mb_uint8_t  b;
} mb_color_t;

/**
 * mb_mmap_type_t.
 */
typedef enum {
    MBMMAP_AVAIL    = 1,
    MBMMAP_RESERVED = 2,
    MBMMAP_ACPI     = 3,
    MBMMAP_NVS      = 4,
    MBMMAP_BADRAM   = 5
} mb_mmap_type_t;

/**
 * mb_mmap_t.
 */
struct __mb_mmap_t {
    mb_uint32_t size;
    mb_uint64_t addr;
    mb_uint64_t len;
    mb_uint32_t type;   // --> mb_mmap_type_t
} __attribute__((packed));
typedef struct __mb_mmap_t mb_mmap_t;

/**
 * mb_mod_t.
 */
typedef struct {
    mb_uint32_t start;
    mb_uint32_t end;
    mb_uint32_t cmdline;
    mb_uint32_t pad;
} mb_mod_t;

#endif