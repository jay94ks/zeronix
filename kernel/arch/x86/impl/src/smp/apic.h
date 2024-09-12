#ifndef __ZERONIX_ARCH_X86_IMPL_SMP_APIC_H__
#define __ZERONIX_ARCH_X86_IMPL_SMP_APIC_H__

#include <zeronix/arch/x86/smp/apic.h>

/**
 * initialize APIC and returns non-zero value if successful. 
 */
uint8_t karch_init_apic();

/**
 * returns whether APIC is supported or not.
 */
uint8_t karch_apic_supported();

/**
 * get the current LAPIC's ID. 
 */
uint32_t karch_lapic_id();

/**
 * read an ioapic register. 
 */
uint32_t karch_ioapic_read(uint32_t iobase, uint32_t reg);

/**
 * write an ioapic register.
 */
void karch_ioapic_write(uint32_t iobase, uint32_t reg, uint32_t val);
/**
 * enable pin.
 */
uint32_t karch_ioapic_enable_pin(uint32_t ioapic_addr, uint32_t pin);

/**
 * disable pin.
 */
uint32_t karch_ioapic_disable_pin(uint32_t ioapic_addr, uint32_t pin);

/**
 * read an lapic register.
 */
uint32_t karch_lapic_read(uint32_t iobase);

/**
 * write an lapic register.
 */
void karch_lapic_write(uint32_t iobase, uint32_t val);

/**
 * get the lapic error number.
 */
uint32_t karch_lapic_error();

/**
 * signal the end of interrupt handler to apic.
 */
uint8_t karch_lapic_eoi();

/**
 * enable the specified CPU's LAPIC.
 */
uint8_t karch_lapic_enable(uint8_t cpu);

#endif