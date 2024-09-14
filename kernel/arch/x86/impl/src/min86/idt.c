#include "idt.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/layout.h>

// --
karch_desc_t desc_idt;
karch_gate_t idt[MAX_IDT] __aligned(8);

// --
void karch_init_idt() {
    kmemset(idt, 0, sizeof(idt));

    desc_idt.base = (uint32_t) idt;
    desc_idt.limit = sizeof(idt) - 1;
}

karch_desc_t* karch_get_idt_ptr() {
    return &desc_idt;
}

void karch_reset_idt() {
    kmemset(idt, 0, sizeof(idt));
}

void karch_flush_idt() {
    load_idt(&desc_idt);
}

void karch_set_idt(uint8_t vec, uint32_t offset, uint32_t priv) {
    karch_gate_t* el = &idt[vec];

    el->off_low = offset & 0xffffu;
    el->off_high = offset >> 16;

    el->sel = SEG_SEL(GDT_KERN_CS);
    el->p_dpl_type = priv;
}

void karch_load_idt_gatevec(const struct gatevec* table, uint8_t vec_base) {
    while (table && table->gate) {
        const struct gatevec* now = table++;

        // --> set each gate vector.
        karch_set_idt(now->vec + vec_base, (uint32_t) now->gate,
            PRESENT | INT_GATE_TYPE | (now->priv << DPL_SHIFT));
    }
}