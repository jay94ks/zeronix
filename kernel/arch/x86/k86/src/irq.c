#define __ARCH_X86_INTERNALS__
#include <x86/legacy/except.h>
#include <x86/legacy/i8259.h>
#include <x86/peripherals/apic.h>
#include <x86/k86/irq.h>
#include <x86/k86/tables.h>
#include <x86/klib.h>

#include <zeronix/arch/irq.h>

// --
karch_irq_t* irq_registry[MAX_IRQ];
uint32_t irq_reg_val;

/**
 * IRQ 0 ~ 16: legacy IRQs.
 */

// -- 
void karch_irq_init() {
    kmemset(irq_registry, 0, sizeof(irq_registry));
}

uint8_t karch_irq_count() {
    return MAX_IRQ;
}

void karch_irq_dispatch(uint8_t n) {
    karch_irq_t* irq = irq_registry[n], *next;

    if (!irq) {
        return;
    }

    while (irq) {
        next = irq->next;

        if (irq->handler) {
            irq->handler(irq);
        }

        irq = next;
    }
}

void karch_irq_mask(uint8_t n) {
    if (karch_i8259_check_imcr()) {
        karch_i8259_mask(n);
        return;
    }
    
    karch_ioapic_disable_irq(n);
}

void karch_irq_unmask(uint8_t n) {
    if (karch_i8259_check_imcr()) {
        karch_i8259_unmask(n);
        return;
    }

    karch_ioapic_set_irq(n);
    karch_ioapic_enable_irq(n);
}

uint8_t karch_irq_register(uint8_t n, karch_irq_t* irq) {
    cpu_cli();

    karch_irq_t** dptr = &irq_registry[n];
    uint8_t unmask = irq_registry[n] == 0;

    while (*dptr) {
        if ((*dptr) == irq) {
            cpu_sti();
            return 0;
        }

        dptr = &((*dptr)->next);
    }

    irq->n = n;
    irq->next = 0;

    *dptr = irq;

    if (unmask) {
        karch_irq_unmask(n);
    }

    cpu_sti();
    return 1;
}

uint8_t karch_irq_unregister(karch_irq_t* irq) {
    cpu_cli();

    karch_irq_t** dptr = &irq_registry[irq->n];

    while (*dptr) {
        if ((*dptr) == irq) {
            *dptr = (*dptr)->next;

            if (!irq_registry[irq->n]) {
                karch_irq_mask(irq->n);
            }
            
            cpu_sti();
            return 1;
        }

        dptr = &((*dptr)->next);
    }

    cpu_sti();
    return 0;
}