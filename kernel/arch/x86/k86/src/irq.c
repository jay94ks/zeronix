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
karch_irq_ovr_t* irq_ovrs[MAX_IRQ];
karch_irq_ovr_t irq_default;
uint32_t irq_reg_val;

/**
 * IRQ 0 ~ 16: legacy IRQs.
 */

// -- 
void karch_irq_init() {
    kmemset(irq_registry, 0, sizeof(irq_registry));
    kmemset(irq_ovrs, 0, sizeof(irq_ovrs));
    irq_default.mask = karch_irq_mask_phys;
    irq_default.unmask = karch_irq_unmask_phys;
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

karch_irq_ovr_t* karch_irq_get_override(uint8_t n) {
    if (n >= MAX_IRQ) {
        return 0;
    }

    if (!irq_ovrs[n]) {
        return &irq_default;
    }

    return irq_ovrs[n];
}

void karch_irq_set_override(uint8_t n, karch_irq_ovr_t* ovr) {
    if (n >= MAX_IRQ) {
        return 0;
    }

    if (!ovr) {
        ovr = &irq_default;
    }

    irq_ovrs[n] = ovr;
}

void karch_irq_mask(uint8_t n) {
    karch_irq_ovr_t* ovr = irq_ovrs[n];

    if (ovr && ovr->mask) {
        ovr->mask(n);
        return;
    }

    karch_irq_mask_phys(n);
}

void karch_irq_unmask(uint8_t n) {
    karch_irq_ovr_t* ovr = irq_ovrs[n];

    if (ovr && ovr->unmask) {
        ovr->unmask(n);
        return;
    }

    karch_irq_unmask_phys(n);
}

void karch_irq_mask_phys(uint8_t n) {
    if (n >= MAX_PHYS_IRQ) {
        return;
    }

    if (karch_i8259_check_imcr()) {
        karch_i8259_mask(n);
        return;
    }
    
    karch_ioapic_disable_irq(n);
}

void karch_irq_unmask_phys(uint8_t n) {
    if (n >= MAX_PHYS_IRQ) {
        return;
    }

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