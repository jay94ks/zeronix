#include "gdt.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/layout.h>

// --
karch_desc_t desc_gdt;
karch_seg_t gdt[GDT_MAX] __aligned(8);

// --
void karch_set_gdt(karch_seg_t* seg, uint32_t base, uint32_t size);
void karch_set_gdt_code(karch_seg_t* seg, uint32_t base, uint32_t size, uint32_t priv);

/**
 * initialize GDT. 
 */
void karch_init_gdt() {
    kmemset(gdt, 0, sizeof(gdt));
    
    desc_gdt.base = (uint32_t) gdt;
    desc_gdt.limit = sizeof(gdt) - 1;

    karch_set_gdt_data(&gdt[GDT_LDT], 0, 0, PRIV_KERN);
    gdt[GDT_LDT].access = PRESENT | LDT;

    karch_set_gdt_code(&gdt[GDT_KERN_CS], 0, 0xffffffffu, PRIV_KERN);
    karch_set_gdt_data(&gdt[GDT_KERN_DS], 0, 0xffffffffu, PRIV_KERN);
    
    karch_set_gdt_code(&gdt[GDT_KERN_CS], 0, 0xffffffffu, PRIV_USER);
    karch_set_gdt_data(&gdt[GDT_KERN_DS], 0, 0xffffffffu, PRIV_USER);
}

karch_desc_t* karch_get_gdt_ptr() {
    return &desc_gdt;
}

void karch_flush_gdt() {
    load_gdt(&desc_gdt);
}

karch_seg_t* karch_get_gdt(uint8_t n) {
    if (n >= GDT_MAX) {
        return 0;
    }

    return &gdt[n];
}

void karch_set_gdt(karch_seg_t* seg, uint32_t base, uint32_t size) {
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

void karch_set_gdt_data(karch_seg_t* seg, uint32_t base, uint32_t size, uint32_t priv) {
    karch_set_gdt(seg, base, size);

    seg->access = (priv >> DPL_SHIFT) | (
        PRESENT | SEGMENT | WRITEABLE | ACCESSED
    );
}

void karch_set_gdt_code(karch_seg_t* seg, uint32_t base, uint32_t size, uint32_t priv) {
    karch_set_gdt(seg, base, size);
    
    seg->access = (priv >> DPL_SHIFT) | (
        PRESENT | SEGMENT | EXECUTABLE | READABLE
    );
}