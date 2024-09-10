#include "acpi.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>

// --
uint32_t acpi_rxsdt;    // RSDT or XSDT.
uint32_t acpi_scale;    // 4: 32bit RSDT, 8: 64bit XSDT.

uint16_t acpi_poff_typa;
uint16_t acpi_poff_typb;
uint16_t acpi_pm1a_cnt_blk;
uint16_t acpi_pm1b_cnt_blk;

// --
uint8_t karch_acpi_check_rsdt();
void karch_acpi_prepare_poweroff();

// https://uefi.org/sites/default/files/resources/ACPI_6_1.pdf
// --
#define ACPI_RSDP_SIG       "RSD PTR "
#define ACPI_RSDP_SIGLEN    8
#define ACPI_SCAN_START     0x000e0000
#define ACPI_SCAN_END       0x000fffff

#define ACPI_SDT_SIG        4
#define ACPI_RSDT           "RSDT"
#define ACPI_XSDT           "XSDT"
#define ACPI_MADT           "APIC"
#define ACPI_FADT           "FACP"

// --
uint8_t karch_init_acpi() {
    uint8_t version;

    acpi_pm1a_cnt_blk = 0;
    acpi_pm1b_cnt_blk = 0;

    // --
    void* ptr = karch_acpi_find_rsdp(&version);
    if (!ptr) {
        // --> not supported.
        acpi_rxsdt = 0;
        acpi_scale = 0;
        return 0;
    }

    if (version == 0) {
        acpi_rxsdt = ((karch_acpi_rsdp_t*) ptr)->rsdt;
        acpi_scale = sizeof(uint32_t);
    }

    else {
        acpi_rxsdt = ((karch_acpi_rsdp_t*) ptr)->rsdt;
        acpi_scale = sizeof(uint64_t);
    }

    if (!karch_acpi_check_rsdt()) {
        acpi_rxsdt = 0;
        acpi_scale = 0;
        return 0;
    }

    karch_acpi_prepare_poweroff();
    return 1;
}

void karch_acpi_prepare_poweroff() {
    karch_acpi_fadt_t* fadt = karch_acpi_find_fadt();
    if (!fadt) {
        return;
    }

    karch_acpi_sdt_t* dsdt = (karch_acpi_sdt_t*) fadt->dsdt;
    if (!dsdt) {
        return;
    }

    uint8_t* ptr = (uint8_t*) (dsdt + 1);
    uint8_t* end = ptr + (dsdt->len - 4);
    uint8_t* start = ptr;

    // --> find _S5_ data.
    while (ptr < end && kmemcmp(ptr, "_S5_", 4) != 0)
        ptr++;

    if (ptr >= end || ptr == start) {
        return;
    }

#define AMI_S5_OPCODE_OFF       4
#define AMI_S5_NAME_OFF         (-2)
#define AMI_S5_NAME_OFF_1       (-1)
#define AMI_S5_OPCODE           0x12
#define AMI_S5_NAME_OPCODE      0x08
#define AMI_S5_PACKET_LEN_OFF   5
#define AMI_LEN_ENC_BITMASK     0xc0
#define AMI_LEN_ENC_BITSHIFT    6
#define AMI_LEN_MIN             1
#define AMI_NUM_ELEM            1
#define AMI_BYTE_PREFIX         0x0a
#define AMI_SLP_TYPA_SHIFT      10
#define AMI_SLP_TYPB_SHIFT      10

    if (*(ptr + AMI_S5_OPCODE_OFF) != AMI_S5_OPCODE) {
        return;
    }

    if ((start + (-AMI_S5_NAME_OFF) ||
        (*(ptr + AMI_S5_NAME_OFF) != AMI_S5_NAME_OPCODE ||
         *(ptr + AMI_S5_NAME_OFF + 1) != '\\')) && 
         *(ptr + AMI_S5_NAME_OFF_1) != AMI_S5_NAME_OPCODE) {
        return;
    }

    if ((ptr += AMI_S5_PACKET_LEN_OFF) >= end) {
        return;
    }

	ptr += (
        (*ptr & AMI_LEN_ENC_BITMASK) >> AMI_LEN_ENC_BITSHIFT) 
        + AMI_LEN_MIN + AMI_NUM_ELEM;

    if (ptr >= end) {
        return;
    }

    if (*ptr == AMI_BYTE_PREFIX) {
        ++ptr;
    }

    acpi_poff_typa = (*ptr++) << AMI_SLP_TYPA_SHIFT;
    if (*ptr == AMI_BYTE_PREFIX) {
        ++ptr;
    }

    acpi_poff_typb = (*ptr++) << AMI_SLP_TYPA_SHIFT;

    acpi_pm1a_cnt_blk = fadt->pm1a_cnt_blk;
    acpi_pm1b_cnt_blk = fadt->pm1b_cnt_blk;
}

uint8_t karch_acpi_supported() {
    return acpi_rxsdt && acpi_scale;
}

uint8_t karch_acpi_poweroff() {
    if (!acpi_pm1a_cnt_blk) {
        return 0;
    }

#define SLP_EN_CODE (1 << 13)
    cpu_out16(acpi_pm1a_cnt_blk, acpi_poff_typa | SLP_EN_CODE);
    if (acpi_pm1b_cnt_blk != 0) {
        cpu_out16(acpi_pm1b_cnt_blk, acpi_poff_typb | SLP_EN_CODE);
    }

    return 1;
}

uint8_t karch_acpi_checksum(void* table, uint32_t len) {
    uint8_t ret = 0;
    uint8_t* ptr = (uint8_t*) table;

    while (len--) {
        ret += *ptr++;
    }

    return ret;
}

uint8_t karch_acpi_test(karch_acpi_rsdp_t* rsdp) {
    if (!rsdp || kmemcmp(rsdp->sig, ACPI_RSDP_SIG, ACPI_RSDP_SIGLEN)  != 0) {
        return 0;
    }

    // --> not a valid rsdp.
    if (karch_acpi_checksum(rsdp, 20) != 0) {
        return 0;
    }

    if (rsdp->rev > 1 && karch_acpi_checksum(rsdp, rsdp->len) != 0) {
        return 0;
    }

    return 1;
}

void* karch_acpi_find_rsdp_r(
    uint8_t* version, uint32_t start, uint32_t end)
{
    if (start > end || (end - start) < ACPI_RSDP_SIGLEN) {
        return 0;
    }

    uint32_t end_r = end - ACPI_RSDP_SIGLEN;
    for (uint32_t i = start; i <= end_r; ++i) {
        karch_acpi_rsdp_t* rsdp = (karch_acpi_rsdp_t*) i;

        if (karch_acpi_test(rsdp)) {
            if (version) {
                *version = rsdp->rev == 0 ? 0 : 1;
            }

            return rsdp;
        }
    }

    return 0;
}

void* karch_acpi_find_rsdp(uint8_t* version) {
    uint16_t ebda = 0;
    void* retval = 0;

    // --> 5.2.5.1 Finding the RSDP on IA-PC Systems
    //   : Read 40:0Eh - to find the starting address of the EBDA.
    kmemcpy(0x040e, &ebda, sizeof(ebda));
    if (ebda) {
        ebda <<= 4;
        retval = karch_acpi_find_rsdp_r(version, ebda, ebda + 0x400);

        if (retval) {
            return retval;
        }
    }

    return karch_acpi_find_rsdp_r(version, 
        ACPI_SCAN_START, ACPI_SCAN_END);
}

/**
 * test whether the found RSDT is valid or not. 
 */
uint8_t karch_acpi_check_rsdt() {
    karch_acpi_sdt_t* hdr = (karch_acpi_sdt_t*) acpi_rxsdt;
    if ((acpi_scale == sizeof(uint32_t) && kmemcmp(hdr->sig, ACPI_RSDT, ACPI_SDT_SIG) == 0) ||
        (acpi_scale == sizeof(uint64_t) && kmemcmp(hdr->sig, ACPI_XSDT, ACPI_SDT_SIG) == 0))
    {
        return 1;
    }

    return 0;
}

karch_acpi_sdt_t* karch_acpi_find_entry_r(karch_acpi_sdt_t* sdt, const char* sig4) {
    uint8_t* entry_begin = (uint8_t*) (sdt + 1);
    uint32_t count = (sdt->len - sizeof(karch_acpi_sdt_t)) / acpi_scale;
    
    for (uint32_t i = 0; i < count; ++i) {
        uint8_t* entry_ptr = entry_begin + (i * acpi_scale);
        karch_acpi_sdt_t* entry = 0;

        if (acpi_scale == 4) {
            entry = (karch_acpi_sdt_t*)(*(uint32_t*)entry_ptr);
        } else {
            entry = (karch_acpi_sdt_t*)(*(uint64_t*)entry_ptr);
        }
        
        if (kmemcmp(entry->sig, sig4, ACPI_SDT_SIG) == 0) {
            return entry;
        }
    }

    return 0;
}

karch_acpi_sdt_t* karch_acpi_find_entry(const char* sig4) {
    if (!acpi_rxsdt) {
        return 0;
    }

    return karch_acpi_find_entry_r(
        (karch_acpi_sdt_t*) acpi_rxsdt, sig4
    );
}

karch_acpi_madt_t* karch_acpi_find_madt() {
    return karch_acpi_find_entry(ACPI_MADT);
}

karch_acpi_fadt_t* karch_acpi_find_fadt() {
    return karch_acpi_find_entry(ACPI_FADT);
}

void karch_acpi_madt_iterate(karch_acpi_madt_iter_t* iter, karch_acpi_madt_t* madt) {
    if (!(iter->madt = madt)) {
        iter->madt = madt = karch_acpi_find_madt();
    }

    if (!madt) {
        iter->cursor = 0;
        return;
    }

    iter->cursor = (uint8_t*) (madt + 1);
}

uint8_t karch_acpi_madt_next(karch_acpi_madt_iter_t* iter, karch_acpi_madt_item_t** item) {
    if (!iter || !iter->madt) {
        return 0;
    }

    karch_acpi_madt_t* madt = iter->madt;
    uint32_t len = madt->hdr.len - sizeof(karch_acpi_madt_t);
    uint8_t* end = (uint8_t*) (madt + 1) + len;

    if (iter->cursor >= end) {
        return 0;
    }

    karch_acpi_madt_item_t* now = (karch_acpi_madt_item_t*)(iter->cursor);
    if (item) {
        *item = now;
    }

    iter->cursor += now->len;
    return 1;
}
