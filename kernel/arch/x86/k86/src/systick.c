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
#define SYSTICK_I8253_REQUIRED_HZ       100000           /* 0.000 01 sec: 100 us. */
#define SYSTICK_APIC_REQUIRED_HZ        100000           /* 0.000 01 sec: 100 us. */
#define SYSTICK_APIC_REQUIRED_FREQ      (SYSTICK_APIC_REQUIRED_HZ / 10)

// --
void karch_systick_apic_handler(const karch_lapic_intr_t* intr);
void karch_systick_irq_handler(karch_irq_t* irq);

// --
karch_systick_t systick;
karch_irq_t systick_irq;
uint32_t systick_freq;

// --
void karch_systick_init() {
    systick = 0;
    systick_freq = 0;

    return;

    karch_lapic_t* lapic = karch_lapic_get_current();
    if (!lapic) {
        systick_freq = SYSTICK_I8253_REQUIRED_HZ;
        systick_irq.handler = karch_systick_irq_handler;

        // --> initialize i8253 timer.
        karch_i8253_init(SYSTICK_I8253_REQUIRED_HZ);
        
        // --> register legacy timer interrupt.
        karch_irq_register(IRQN_TIMER, &systick_irq);
        return;
    }

    systick_freq = SYSTICK_APIC_REQUIRED_HZ;

    // --> use APIC timer: setup periodic timer here.
    karch_apic_set_handler(LAPICIRQ_TIMER, karch_systick_apic_handler);
    karch_lapic_periodic_timer(SYSTICK_APIC_REQUIRED_FREQ); // --> 1 usec.

    cpu_sti();
}

/**
 * set the `systick` handler.
 */
void karch_systick_set_handler(karch_systick_t handler) {
    systick = handler;
    cpu_mfence();
}

/**
 * pass the timer interrupt to SYSTICK handler.
 */
void karch_systick_irq_handler(karch_irq_t* irq) {
    karch_systick_t exec = systick;

    // --> i8259 is always under BSP CPU.
    if (exec) {
        exec();
    }
}

/**
 * pass the timer interrupt to SYSTICK handler.
 */
void karch_systick_apic_handler(const karch_lapic_intr_t* intr) {
    uint8_t is_bsp = karch_smp_bspid() == karch_smp_cpuid() ? 1 : 0;
    karch_systick_t exec = systick;

    karch_lapic_stop_timer();

    if (exec) {
        exec();
    }

    karch_lapic_periodic_timer(SYSTICK_APIC_REQUIRED_FREQ);
}