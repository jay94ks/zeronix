#define __ARCH_X86_INTERNALS__ 1
#define __NO_SYSTICK_EXTERNS__ 1
#include <x86/k86/systick.h>
#include <x86/k86/irq.h>
#include <x86/smp.h>

#include <x86/legacy/i8253.h>
#include <x86/legacy/i8259.h>

#include <x86/peripherals/apic.h>
#include <x86/klib.h>

// --
#define SYSTICK_I8253_REQUIRED_HZ       10000
#define SYSTICK_APIC_REQUIRED_HZ        10000
#define SYSTICK_APIC_REQUIRED_FREQ      (SYSTICK_APIC_REQUIRED_HZ / 10)

// --
karch_irq_ovr_t systick_ovr;
karch_irq_t systick_irq;
uint32_t systick_freq;
uint8_t systick_mode;                   // 0: i8253, 1: apic.

// --
void karch_systick_mask(uint8_t);
void karch_systick_unmask(uint8_t);

/* redirect i8259 timer interrupt to SYSTICK. */
void karch_systick_irq_handler(karch_irq_t*) {
    karch_irq_dispatch(IRQN_SYSTICK);
}

// --
void karch_systick_init() {
    karch_lapic_t* lapic = karch_lapic_get_current();

    systick_ovr.mask = karch_systick_mask;
    systick_ovr.unmask = karch_systick_unmask;
    karch_irq_set_override(IRQN_SYSTICK, &systick_ovr);

    systick_mode = lapic ? 1 : 0;
    systick_freq = lapic 
        ? SYSTICK_APIC_REQUIRED_HZ 
        : SYSTICK_I8253_REQUIRED_HZ;

    if (!systick_mode) {
        systick_irq.handler = karch_systick_irq_handler;
    }
}

void karch_systick_mask(uint8_t) {
    if (systick_mode) {
        karch_lapic_stop_timer();
        return;
    }

    karch_i8253_deinit();
    karch_irq_unregister(&systick_irq);
}

void karch_systick_unmask(uint8_t) {
    if (systick_mode) {
        karch_lapic_periodic_timer(SYSTICK_APIC_REQUIRED_FREQ); // --> 1 usec.
        return;
    }

    karch_irq_register(IRQN_TIMER, &systick_irq);
    karch_i8253_init(SYSTICK_I8253_REQUIRED_HZ);
}