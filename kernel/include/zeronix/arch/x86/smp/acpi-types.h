#ifndef __ZERONIX_ARCH_X86_SMP_ACPI_TYPES_H__
#define __ZERONIX_ARCH_X86_SMP_ACPI_TYPES_H__

#include <zeronix/types.h>

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

/* shorthands. */
typedef struct karch_x86_acpi_madt_ioapic karch_acpi_madt_ioapic_t;
typedef struct karch_x86_acpi_madt_nmi karch_acpi_madt_nmi_t;

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
#endif