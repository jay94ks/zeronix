#define __ARCH_X86_INTERNALS__
#include <x86/k86/tables.h>
#include <x86/klib.h>

#include <zeronix/kstring.h>

// --
karch_desc_t desc_idt;
karch_desc_t desc_gdt;

// --
karch_gate_t idt[MAX_IDT] __aligned(8);
karch_seg_t gdt[GDT_MAX] __aligned(8);

// --
void karch_tables_gdt_set(karch_seg_t* seg, uint32_t base, uint32_t size);
void karch_tables_gdt_code(karch_seg_t* seg, uint32_t base, uint32_t size, uint32_t priv);

/**
 * initialize tables.
*/
void karch_tables_init() {
    kmemset(gdt, 0, sizeof(gdt));
    karch_tables_reset_idt();

    desc_gdt.base = (uint32_t) gdt;
    desc_gdt.limit = sizeof(gdt) - 1;

    desc_idt.base = (uint32_t) idt;
    desc_idt.limit = sizeof(idt) - 1;
    
    // --
    karch_tables_gdt_data(&gdt[GDT_LDT], 0, 0, PRIV_KERN);
    gdt[GDT_LDT].access = PRESENT | LDT;

    karch_tables_gdt_code(&gdt[GDT_KERN_CS], 0, 0xffffffffu, PRIV_KERN);
    karch_tables_gdt_data(&gdt[GDT_KERN_DS], 0, 0xffffffffu, PRIV_KERN);
    
    karch_tables_gdt_code(&gdt[GDT_USER_CS], 0, 0xffffffffu, PRIV_USER);
    karch_tables_gdt_data(&gdt[GDT_USER_DS], 0, 0xffffffffu, PRIV_USER);
}

void karch_tables_get_descriptors(karch_table_desc_t* tables) {
    if (tables) {
        tables->gdt = desc_gdt;
        tables->idt = desc_idt;
    }
}

karch_seg_t* karch_tables_gdt_ptr(uint8_t n) {
    if (n >= GDT_MAX) {
        return 0;
    }

    return &gdt[n];
}

void karch_tables_flush_gdt() {
    load_gdt(&desc_gdt);
}

void karch_tables_flush_idt() {
    load_idt(&desc_idt);
}

void karch_tables_gdt_set(karch_seg_t* seg, uint32_t base, uint32_t size) {
    seg->base_low = base & 0xffff;
    seg->base_mid = (base >> 16) & 0xff;
    seg->base_high = (base >> 24) & 0xff;

    --size;
    if (size > BYTE_GRAN_MAX) {
        seg->limit_low = size >> GRANULARITY_SHIFT;
        seg->granularity = GRANULAR | (size >> PAGE_GRAN_SHIFT);
    } else {
        seg->limit_low = size;
        seg->granularity = size >> GRANULARITY_SHIFT;
    }

    seg->granularity |= DEFAULT;

}

void karch_tables_gdt_data(karch_seg_t* seg, uint32_t base, uint32_t size, uint32_t priv) {
    karch_tables_gdt_set(seg, base, size);

    seg->access = (priv >> DPL_SHIFT) | (
        PRESENT | SEGMENT | WRITEABLE | ACCESSED
    );
}

void karch_tables_gdt_code(karch_seg_t* seg, uint32_t base, uint32_t size, uint32_t priv) {
    karch_tables_gdt_set(seg, base, size);
    
    seg->access = (priv >> DPL_SHIFT) | (
        PRESENT | SEGMENT | EXECUTABLE | READABLE
    );
}

void karch_tables_reset_idt() {
    kmemset(idt, 0, sizeof(idt));
}

void karch_tables_idt(uint8_t vec, uint32_t offset, uint32_t priv) {
    karch_gate_t* el = &idt[vec];

    el->off_low = offset & 0xffffu;
    el->off_high = offset >> 16;

    el->sel = SEG_SEL(GDT_KERN_CS);
    el->p_dpl_type = priv;
}

void karch_tables_load_idt(const struct gatevec* table, uint8_t vec_base) {
    while (table && table->gate) {
        const struct gatevec* now = table++;

        // --> set each gate vector.
        karch_tables_idt(now->vec + vec_base, (uint32_t) now->gate,
            PRESENT | INT_GATE_TYPE | (now->priv << DPL_SHIFT));
    }
}
