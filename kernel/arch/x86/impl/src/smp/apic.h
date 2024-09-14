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

/**
 * set oneshot timer that is based on LAPIC.
 * this works with `karch_get_cpu_current`.
 * so, this can be called after it is ready.
 */
uint8_t karch_lapic_oneshot_timer(uint32_t usec);

/**
 * set periodic timer that is based on LAPIC.
 * this works with `karch_get_cpu_current`.
 * so, this can be called after it is ready.
 */
uint8_t karch_lapic_periodic_timer(uint32_t freq);

/**
 * stop the LAPIC timer.
 * this is available after lapic initialized.
 */
void karch_lapic_stop_timer();

/**
 * micro-delay based on LAPIC timer.
 * this works with `karch_get_cpu_current`.
 * so, this can be called after it is ready.
 */
void karch_lapic_udelay(uint32_t n);

/**
 * milli-delay based on LAPIC timer.
 * this works with `karch_get_cpu_current`.
 * so, this can be called after it is ready.
 */
void karch_lapic_mdelay(uint32_t ms);

/**
 * result codes for `wait_flag_*` functions.
 * karch_lapic_wr_t: wait result type.
*/
typedef enum {
    LAPICWR_SUCCESS = 0,
    LAPICWR_EXHAUSTED = 1,
    LAPICWR_ERROR = 2,
} karch_lapic_wr_t;

/**
 * wait for LAPIC flag to be set until `tries`.
 * this is available after lapic initialized.
 * ----
 * returns: karch_lapic_waitresult_t.
 */
karch_lapic_wr_t karch_lapic_wait_flag_set(uint32_t addr, uint32_t flag, uint32_t tries);

/**
 * wait for LAPIC flag to be cleared until `tries`.
 * this is available after lapic initialized.
 * ----
 * returns: karch_lapic_waitresult_t.
 */
karch_lapic_wr_t karch_lapic_wait_flag_clear(uint32_t addr, uint32_t flag, uint32_t tries);

#endif