#ifndef __ZERONIX_ARCH_X86_IMPL_SMP_ACPI_H__
#define __ZERONIX_ARCH_X86_IMPL_SMP_ACPI_H__

#include <zeronix/arch/x86/smp/acpi.h>

/**
 * initialize ACPI and returns non-zero if succeed.
 * returns non-zero if successful.
*/
uint8_t karch_init_acpi();

/**
 * returns whether ACPI is supported or not.
 * this valid after `karch_init_acpi` call.
 */
uint8_t karch_acpi_supported();

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

/**
 * test checksum of ACPI element.
 * returns zero if checksum matches.
 */
uint8_t karch_acpi_checksum(void* table, uint32_t len);

/**
 * find RSDP/XSDP from entire system.
 */
void* karch_acpi_find_rsdp(uint8_t* version);

/**
 * find ACPI's RSDP/XSDP and prepare RSDT entity.
 * returns non-zero if successful.
 */
uint8_t karch_acpi_init_rsdt();

/**
 * find an SDT entry recursively.
 * sig4 must be 4 letters.
 */
karch_acpi_sdt_t* karch_acpi_find_entry(const char* sig4);

/**
 * find MADT entry.
 */
karch_acpi_madt_t* karch_acpi_find_madt();

/**
 * find FADT entry.
 */
karch_acpi_fadt_t* karch_acpi_find_fadt();

/**
 * karch_acpi_madt_iter_t.
 * MADT item iterator.
*/
typedef struct {
    karch_acpi_madt_t* madt;
    uint8_t* cursor;
} karch_acpi_madt_iter_t;

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


#endif 