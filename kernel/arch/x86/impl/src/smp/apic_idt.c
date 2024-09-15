#define __ARCH_X86_INTERNALS__  1

#include "apic.h"
#include "acpi.h"
#include "../min86/idt.h"
#include "../min86/i8259.h"
#include "../min86/except.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/spinlock.h>
#include <zeronix/arch/x86/cpu.h>
#include <zeronix/arch/x86/layout.h>

#if 1
void karch_apic_hwint_00();
void karch_apic_hwint_01();
void karch_apic_hwint_02();
void karch_apic_hwint_03();
void karch_apic_hwint_04();
void karch_apic_hwint_05();
void karch_apic_hwint_06();
void karch_apic_hwint_07();
void karch_apic_hwint_08();
void karch_apic_hwint_09();

void karch_apic_hwint_10();
void karch_apic_hwint_11();
void karch_apic_hwint_12();
void karch_apic_hwint_13();
void karch_apic_hwint_14();
void karch_apic_hwint_15();
void karch_apic_hwint_16();
void karch_apic_hwint_17();
void karch_apic_hwint_18();
void karch_apic_hwint_19();

void karch_apic_hwint_20();
void karch_apic_hwint_21();
void karch_apic_hwint_22();
void karch_apic_hwint_23();
void karch_apic_hwint_24();
void karch_apic_hwint_25();
void karch_apic_hwint_26();
void karch_apic_hwint_27();
void karch_apic_hwint_28();
void karch_apic_hwint_29();

void karch_apic_hwint_30();
void karch_apic_hwint_31();
void karch_apic_hwint_32();
void karch_apic_hwint_33();
void karch_apic_hwint_34();
void karch_apic_hwint_35();
void karch_apic_hwint_36();
void karch_apic_hwint_37();
void karch_apic_hwint_38();
void karch_apic_hwint_39();

void karch_apic_hwint_40();
void karch_apic_hwint_41();
void karch_apic_hwint_42();
void karch_apic_hwint_43();
void karch_apic_hwint_44();
void karch_apic_hwint_45();
void karch_apic_hwint_46();
void karch_apic_hwint_47();
void karch_apic_hwint_48();
void karch_apic_hwint_49();

void karch_apic_hwint_50();
void karch_apic_hwint_51();
void karch_apic_hwint_52();
void karch_apic_hwint_53();
void karch_apic_hwint_54();
void karch_apic_hwint_55();
void karch_apic_hwint_56();
void karch_apic_hwint_57();
void karch_apic_hwint_58();
void karch_apic_hwint_59();

void karch_apic_hwint_60();
void karch_apic_hwint_61();
void karch_apic_hwint_62();
void karch_apic_hwint_63();

void karch_apic_zbint_f0();
void karch_apic_zbint_f1();
void karch_apic_zbint_f2();
void karch_apic_zbint_fe();
void karch_apic_zbint_ff();
#endif

static const struct gatevec gv_apic_hw[] = {
    { karch_apic_hwint_00,  0, PRIV_KERN },
    { karch_apic_hwint_01,  1, PRIV_KERN },
    { karch_apic_hwint_02,  2, PRIV_KERN },
    { karch_apic_hwint_03,  3, PRIV_KERN },
    { karch_apic_hwint_04,  4, PRIV_KERN },
    { karch_apic_hwint_05,  5, PRIV_KERN },
    { karch_apic_hwint_06,  6, PRIV_KERN },
    { karch_apic_hwint_07,  7, PRIV_KERN },
    { karch_apic_hwint_08,  8, PRIV_KERN },
    { karch_apic_hwint_09,  9, PRIV_KERN },
    
    { karch_apic_hwint_10, 10, PRIV_KERN },
    { karch_apic_hwint_11, 11, PRIV_KERN },
    { karch_apic_hwint_12, 12, PRIV_KERN },
    { karch_apic_hwint_13, 13, PRIV_KERN },
    { karch_apic_hwint_14, 14, PRIV_KERN },
    { karch_apic_hwint_15, 15, PRIV_KERN },
    { karch_apic_hwint_16, 16, PRIV_KERN },
    { karch_apic_hwint_17, 17, PRIV_KERN },
    { karch_apic_hwint_18, 18, PRIV_KERN },
    { karch_apic_hwint_19, 19, PRIV_KERN },
    
    { karch_apic_hwint_20, 20, PRIV_KERN },
    { karch_apic_hwint_21, 21, PRIV_KERN },
    { karch_apic_hwint_22, 22, PRIV_KERN },
    { karch_apic_hwint_23, 23, PRIV_KERN },
    { karch_apic_hwint_24, 24, PRIV_KERN },
    { karch_apic_hwint_25, 25, PRIV_KERN },
    { karch_apic_hwint_26, 26, PRIV_KERN },
    { karch_apic_hwint_27, 27, PRIV_KERN },
    { karch_apic_hwint_28, 28, PRIV_KERN },
    { karch_apic_hwint_29, 29, PRIV_KERN },
    
    { karch_apic_hwint_30, 30, PRIV_KERN },
    { karch_apic_hwint_31, 31, PRIV_KERN },
    { karch_apic_hwint_32, 32, PRIV_KERN },
    { karch_apic_hwint_33, 33, PRIV_KERN },
    { karch_apic_hwint_34, 34, PRIV_KERN },
    { karch_apic_hwint_35, 35, PRIV_KERN },
    { karch_apic_hwint_36, 36, PRIV_KERN },
    { karch_apic_hwint_37, 37, PRIV_KERN },
    { karch_apic_hwint_38, 38, PRIV_KERN },
    { karch_apic_hwint_39, 39, PRIV_KERN },
        
    { karch_apic_hwint_40, 40, PRIV_KERN },
    { karch_apic_hwint_41, 41, PRIV_KERN },
    { karch_apic_hwint_42, 42, PRIV_KERN },
    { karch_apic_hwint_43, 43, PRIV_KERN },
    { karch_apic_hwint_44, 44, PRIV_KERN },
    { karch_apic_hwint_45, 45, PRIV_KERN },
    { karch_apic_hwint_46, 46, PRIV_KERN },
    { karch_apic_hwint_47, 47, PRIV_KERN },
    { karch_apic_hwint_48, 48, PRIV_KERN },
    { karch_apic_hwint_49, 49, PRIV_KERN },
    
    { karch_apic_hwint_50, 50, PRIV_KERN },
    { karch_apic_hwint_51, 51, PRIV_KERN },
    { karch_apic_hwint_52, 52, PRIV_KERN },
    { karch_apic_hwint_53, 53, PRIV_KERN },
    { karch_apic_hwint_54, 54, PRIV_KERN },
    { karch_apic_hwint_55, 55, PRIV_KERN },
    { karch_apic_hwint_56, 56, PRIV_KERN },
    { karch_apic_hwint_57, 57, PRIV_KERN },
    { karch_apic_hwint_58, 58, PRIV_KERN },
    { karch_apic_hwint_59, 59, PRIV_KERN },
    
    { karch_apic_hwint_60, 60, PRIV_KERN },
    { karch_apic_hwint_61, 61, PRIV_KERN },
    { karch_apic_hwint_62, 62, PRIV_KERN },
    { karch_apic_hwint_63, 63, PRIV_KERN },

    GATEVEC_NULL
};

static const struct gatevec gv_apic_zb[] = {
    { karch_apic_zbint_f1, 0xf1, PRIV_KERN }, // --> sched proc.  (SMP only)
    { karch_apic_zbint_f2, 0xf2, PRIV_KERN }, // --> CPU halt.    (SMP only)
    { karch_apic_zbint_fe, 0xff, PRIV_KERN }, // --> spurious int vec.
    { karch_apic_zbint_ff, 0xfe, PRIV_KERN }, // --> error int vec.
    GATEVEC_NULL
};

// --> BSP ONLY.
static const struct gatevec gv_apic_bsp[] = {
    { karch_apic_zbint_f1, 0xf1, PRIV_KERN }, // --> timer.  (SMP only)
    GATEVEC_NULL
};

void karch_init_apic_idt() {
    uint8_t is_bsp = karch_is_bsp_cpu_current();

    // --> load hardware interrupt, + 0x50.
    karch_load_idt_gatevec(gv_apic_hw, 0x50);

    // --> load special interrupts, as-is.
    karch_load_idt_gatevec(gv_apic_zb, 0x00);

    // --> error interrupt vector.
    uint32_t val = karch_lapic_read(LAPIC_LVTER);
    val |= APIC_ERROR_INT_VECTOR;
    val &= ~ APIC_ICR_INT_MASK;
    karch_lapic_write(LAPIC_LVTER, val);
	(void)  karch_lapic_read(LAPIC_LVTER);

    // --> timer interrupt vector.
	if (is_bsp) {
        karch_load_idt_gatevec(gv_apic_bsp, 0x00);
	}
}

void karch_deinit_apic_idt() {
    // --> reset IDT.
    karch_reset_idt();
    
    // --> then, load min86's PIC interrupt vectors.
    karch_reload_i8259_min86();

    // --> flush IDT table.
    karch_flush_idt();

    // --> then, re-enable ICMR of i8259.
    karch_i8259_icmr_enable();
}

/**
 * Called from apic_idt.asm.
 * `n` value: refer `gv_apic_hw`.
*/
void karch_apic_hwint(uint32_t n, uint32_t k, karch_intr_frame_t* frame) {

}

/**
 * Called from apic_idt.asm.
 * `n` value: refer `gv_apic_zb`.
*/
void karch_apic_zbint(uint32_t n, uint32_t k, karch_intr_frame_t* frame) {
    switch (n) {
        case 0xf0: // --> timer. (only issued in BSP)
            break;

        case 0xf1: // 
        case 0xf2:
        case 0xfe:
        case 0xff:
            break;
    }
}
