// kernel_arch_x86_inc_x86_peripherals_acpi_h
// PRESS TAB TO GENERATE COMPLETELY.

#ifndef __KERNEL_ARCH_X86_INC_X86_PERIPHERALS_ACPI_H__
#define __KERNEL_ARCH_X86_INC_X86_PERIPHERALS_ACPI_H__

#include <zeronix/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * karch_x86_acpi_rsdp.
 */
struct karch_x86_acpi_rsdp {
    char        sig[8];
    uint8_t     chk;
    char        oem[6];
    uint8_t     rev;        // 0 = rsdp, 1 = xsdp.
    uint32_t    rsdt;
    uint32_t    len;
} __packed;

/**
 * karch_x86_acpi_xsdp.
 */
struct karch_x86_acpi_xsdp {
    char        sig[8];
    uint8_t     chk;
    char        oem[6];
    uint8_t     rev;        // 0 = rsdp, 1 = xsdp.
    uint32_t    rsdt;       // --> deprecated.
    // --
    uint32_t    len;
    uint64_t    xsdt;
    uint8_t     echk;       // --> extended checksum.
    uint8_t     resv[3];
} __packed;

/* shorthand. */
typedef struct karch_x86_acpi_rsdp karch_acpi_rsdp_t;
typedef struct karch_x86_acpi_xsdp karch_acpi_xsdp_t;

/**
 * karch_x86_acpi_sdt.
 */
struct karch_x86_acpi_sdt {
    char        sig[4];
    uint32_t    len;
    uint8_t     rev;
    uint8_t     chk;
    char        oem[6];
    char        oemti[8];
    uint32_t    oem_rev;
    uint32_t    ctor_id;
    uint32_t    ctor_rev;
} __packed;

/* shorthand.*/
typedef struct karch_x86_acpi_sdt karch_acpi_sdt_t;

/**
 * karch_x86_acpi_madt.
 */
struct karch_x86_acpi_madt {
    karch_acpi_sdt_t    hdr;
    uint32_t            addr;       // --> local APIC address.
    uint32_t            flags;      // --> if bit 0 set, i8259 must be disabled.

    /* items here. */
} __packed;

/**
 * karch_x86_acpi_item.
 */
struct karch_x86_acpi_madt_item {
    uint8_t         type;
    uint8_t         len;
} __packed;

/* shorthand.*/
typedef struct karch_x86_acpi_madt karch_acpi_madt_t;
typedef struct karch_x86_acpi_madt_item karch_acpi_madt_item_t;

/**
 * type value for karch_acpi_madt_item.
*/
typedef enum {
    ACPIMADT_LAPIC = 0,
    ACPIMADT_IOAPIC = 1,
    ACPIMADT_INT_SRC = 2,
    ACPIMADT_NMI_SRC = 3,
    ACPIMADT_LAPIC_NMI = 4,
    ACPIMADT_LAPIC_ADDR = 5,
    ACPIMADT_IOSAPIC = 6,
    ACPIMADT_LSAPIC = 7,
    ACPIMADT_PLATFORM_INT_SRC = 8,
    ACPIMADT_Lx2APIC = 9,
    ACPIMADT_Lx2APIC_NMI = 10
} karch_acpi_madt_type_t;

/**
 * karch_x86_acpi_madt_lapic.
 */
struct karch_x86_acpi_madt_lapic {
    karch_acpi_madt_item_t  hdr;
    uint8_t                 cpu;
    uint8_t                 apic;
    uint32_t                flags;
} __packed;

/**
 * karch_x86_acpi_madt_lapic.
 */
struct karch_x86_acpi_madt_ioapic {
    karch_acpi_madt_item_t  hdr;
    uint8_t                 id;
    uint8_t                 rsv;
    uint32_t                addr;
    uint32_t                intr;   // --> global interrupt base.
} __packed;

/**
 * karch_x86_acpi_madt_nmi.
 */
struct karch_x86_acpi_madt_nmi {
    karch_acpi_madt_item_t  hdr;
    uint16_t                flags;
    uint32_t                intr;   // --> global interrupt.
} __packed;

/**
 * https://uefi.org/sites/default/files/resources/ACPI_6_3_final_Jan30.pdf
 * page. 155.
 */
struct karch_x86_acpi_madt_intsrc {
    karch_acpi_madt_item_t  hdr;
    uint8_t bus;    // --> constant, zero. --> ISA.
    uint8_t source; // --> bus relative IRQn.
    uint32_t global_intr;   // --> how it remapped as.
    
    uint16_t flags;
} __packed;

#define ACPI_INTSRC_GET_POLARITY(x)     ((x) & 0x03)
#define ACPI_INTSRC_GET_TRIGGER_MODE(x) (((x) >> 2) & 0x03)

/* shorthands. */
typedef struct karch_x86_acpi_madt_lapic karch_acpi_madt_lapic_t;
typedef struct karch_x86_acpi_madt_ioapic karch_acpi_madt_ioapic_t;
typedef struct karch_x86_acpi_madt_nmi karch_acpi_madt_nmi_t;
typedef struct karch_x86_acpi_madt_intsrc karch_acpi_madt_intsrc_t;

/**
 * polarity flags: ACPI_INTSRC_GET_POLARITY(x).
 */
typedef enum {
    INTSRC_POL_BUSSPEC = 0,
    INTSRC_POL_ACTIVEHIGH = 1,
    INTSRC_POL_RESERVED = 2,
    INTSRC_POL_ACTIVELOW = 3
} karch_acpi_intsrc_polarity_t;

/**
 * polarity flags: ACPI_INTSRC_GET_POLARITY(x).
 */
typedef enum {
    INTSRC_TRG_BUSSPEC = 0,
    INTSRC_TRG_EDGE = 1,
    INTSRC_TRG_RESERVED = 2,
    INTSRC_TRG_LEVEL = 3
} karch_acpi_intsrc_trigger_t;

/**
 * karch_x86_acpi_gaddr.
 */
struct karch_x86_acpi_gaddr {
    uint8_t             addr_space_id;
    uint8_t             reg_bit_width;
    uint8_t             reg_bit_offset;
    uint8_t             access_size;
    uint8_t             addr;
} __packed;

/* shorthands. */
typedef struct karch_x86_acpi_gaddr karch_acpi_gaddr_t;

/**
 * karch_x86_acpi_fadt.
 */
struct karch_x86_acpi_fadt {
    karch_acpi_sdt_t    hdr;

    uint32_t facs;
    uint32_t dsdt;
    uint8_t model;
    uint8_t ppp;    // --> preferred pm profile.

    uint16_t sci_int;
    uint32_t smi_cmd;
    uint8_t acpi_enable;
    uint8_t acpi_disable;

    uint8_t s4bios_req;
    uint8_t pstate_cnt;

	uint32_t pm1a_evt_blk;
	uint32_t pm1b_evt_blk;
	uint32_t pm1a_cnt_blk;
	uint32_t pm1b_cnt_blk;
	uint32_t pm2_cnt_blk;
	uint32_t pm_tmr_blk;
    
	uint32_t gpe0_blk;
	uint32_t gpe1_blk;

	uint8_t pm1_evt_len;
	uint8_t pm1_cnt_len;
	uint8_t pm2_cnt_len;
	uint8_t pm_tmr_len;

	uint8_t gpe0_blk_len;
	uint8_t gpe1_blk_len;
	uint8_t gpe1_base;

	uint8_t cst_cnt;
	uint16_t p_lvl2_lat;
	uint16_t p_lvl3_lat;
	uint16_t flush_size;
	uint16_t flush_stride;
	uint8_t duty_offset;
	uint8_t duty_width;
	uint8_t day_alrm;
	uint8_t mon_alrm;
	uint8_t century;

	uint16_t iapc_boot_arch;
	uint8_t resv1;
	uint32_t flags;

	karch_acpi_gaddr_t reset_reg;

	uint8_t reset_value;
	uint8_t resv2[3];
	uint64_t xfacs;
	uint64_t xdsdt;

	karch_acpi_gaddr_t xpm1a_evt_blk;
	karch_acpi_gaddr_t xpm1b_evt_blk;
	karch_acpi_gaddr_t xpm1a_cnt_blk;
	karch_acpi_gaddr_t xpm1b_cnt_blk;
	karch_acpi_gaddr_t xpm2_cnt_blk;
	karch_acpi_gaddr_t xpm_tmr_blk;
	karch_acpi_gaddr_t xgpe0_blk;
	karch_acpi_gaddr_t xgpe1_blk;
} __packed;

/* shorthands. */
typedef struct karch_x86_acpi_fadt karch_acpi_fadt_t;

/**
 * karch_acpi_madt_iter_t.
 * MADT item iterator.
*/
typedef struct {
    karch_acpi_madt_t* madt;
    uint8_t* cursor;
} karch_acpi_madt_iter_t;

/**
 * returns whether ACPI is supported or not.
 * this valid after `karch_init_acpi` call.
 */
uint8_t karch_acpi_supported();

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize ACPI and returns non-zero if succeed.
 * returns non-zero if successful.
 */
uint8_t karch_acpi_init();
#endif 

/**
 * test checksum of ACPI element.
 * returns zero if checksum matches.
 */
uint8_t karch_acpi_checksum(void* table, uint32_t len);


/**
 * find an SDT entry recursively.
 * sig4 must be 4 letters.
 */
karch_acpi_sdt_t* karch_acpi_find_entry(const char* sig4);

// --
#define ACPI_MADT           "APIC"
#define ACPI_FADT           "FACP"

/**
 * find MADT entry.
 */
#define karch_acpi_find_madt() \
    ((karch_acpi_madt_t*)karch_acpi_find_entry(ACPI_MADT))

/**
 * find FADT entry.
 */
#define karch_acpi_find_fadt() \
    ((karch_acpi_fadt_t*)karch_acpi_find_entry(ACPI_FADT))

/**
 * make MADT iterator.
 */
void karch_acpi_madt_iterate(karch_acpi_madt_iter_t* iter, karch_acpi_madt_t* madt);

/**
 * get next item using iterator.
 * returns non-zero value if item could be read.
 * ----
 * usage:
 * 
 * karch_acpi_madt_iter_t iter;
 * karch_acpi_madt_item_t* item;
 * 
 * karch_acpi_madt_iterate(&iter, madt);
 * while (karch_acpi_madt_next(&iter, &item)) {
 *  // --> item is available.
 * }
 * 
 */
uint8_t karch_acpi_madt_next(karch_acpi_madt_iter_t* iter, karch_acpi_madt_item_t** item);

/**
 * reboot the machine by ACPI.
 * returns non-zero if supported and executed.
 */
uint8_t karch_acpi_reboot();

/**
 * turn off the machine by ACPI. 
 * returns non-zero if supported and executed.
 */
uint8_t karch_acpi_poweroff();

#ifdef __cplusplus
}
#endif
#endif