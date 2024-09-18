#define __ARCH_X86_INTERNALS__ 1
#define __NO_SYSTICK_EXTERNS__ 1
#include <x86/k86/systick.h>
#include <x86/smp.h>

#include <x86/legacy/i8253.h>
#include <x86/legacy/i8259.h>

#include <x86/peripherals/apic.h>
#include <x86/klib.h>

// --
#define SYSTICK_I8253_REQUIRED_HZ       100000           /* 0.000 01 sec: 100 us. */
#define SYSTICK_APIC_REQUIRED_HZ        100000           /* 100 us: 0.01 ms. */
#define SYSTICK_APIC_REQUIRED_FREQ      100000 / 10      /* 100 us: 0.01 ms. */

// --
void karch_systick_i8259_handler(const karch_i8259_t* intr);
void karch_systick_apic_handler(const karch_lapic_intr_t* intr);

// --
karch_systick_t systick;
uint32_t systick_freq;

// --
void karch_systick_init() {
    systick = 0;
    systick_freq = 0;

    karch_lapic_t* lapic = karch_lapic_get_current();
    if (!lapic) {
        systick_freq = SYSTICK_I8253_REQUIRED_HZ;

        // --> use i8259 timer.
        karch_i8259_set_handler(I8259_TIMER, karch_systick_i8259_handler, 0);

        // --> initialize i8253 timer.
        karch_i8253_init(SYSTICK_I8253_REQUIRED_HZ);
        karch_i8259_unmask(I8259_TIMER);

        cpu_sti();
        return;
    }

    systick_freq = SYSTICK_APIC_REQUIRED_HZ;

    // --> use APIC timer: setup periodic timer here.
    karch_apic_set_handler(LAPICIRQ_TIMER, karch_systick_apic_handler);
    karch_lapic_periodic_timer(SYSTICK_APIC_REQUIRED_FREQ);

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
void karch_systick_i8259_handler(const karch_i8259_t* intr) {
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
    
    if (exec) {
        exec();
    }
}