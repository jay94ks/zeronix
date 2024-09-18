#define __ARCH_X86_INTERNALS__ 1
#include <x86/legacy/i8259.h>
#include <x86/k86/tables.h>
#include <x86/klib.h>

#include <zeronix/kstring.h>

#if 1
extern void karch_i8259_hwint_00();
extern void karch_i8259_hwint_01();
extern void karch_i8259_hwint_02();
extern void karch_i8259_hwint_03();
extern void karch_i8259_hwint_04();
extern void karch_i8259_hwint_05();
extern void karch_i8259_hwint_06();
extern void karch_i8259_hwint_07();

extern void karch_i8259_hwint_08();
extern void karch_i8259_hwint_09();
extern void karch_i8259_hwint_10();
extern void karch_i8259_hwint_11();
extern void karch_i8259_hwint_12();
extern void karch_i8259_hwint_13();
extern void karch_i8259_hwint_14();
extern void karch_i8259_hwint_15();
#endif

// --
static const struct gatevec gv_i8259_master[] = {
    { karch_i8259_hwint_00, 0, PRIV_KERN },
    { karch_i8259_hwint_01, 1, PRIV_KERN },
    { karch_i8259_hwint_02, 2, PRIV_KERN },
    { karch_i8259_hwint_03, 3, PRIV_KERN },
    { karch_i8259_hwint_04, 4, PRIV_KERN },
    { karch_i8259_hwint_05, 5, PRIV_KERN },
    { karch_i8259_hwint_06, 6, PRIV_KERN },
    { karch_i8259_hwint_07, 7, PRIV_KERN },
    GATEVEC_NULL
};

static const struct gatevec gv_i8259_slave[] = {
    { karch_i8259_hwint_08, 0, PRIV_KERN },
    { karch_i8259_hwint_09, 1, PRIV_KERN },
    { karch_i8259_hwint_10, 2, PRIV_KERN },
    { karch_i8259_hwint_11, 3, PRIV_KERN },
    { karch_i8259_hwint_12, 4, PRIV_KERN },
    { karch_i8259_hwint_13, 5, PRIV_KERN },
    { karch_i8259_hwint_14, 6, PRIV_KERN },
    { karch_i8259_hwint_15, 7, PRIV_KERN },
    GATEVEC_NULL
};

// --
karch_i8259_cb_t i8259_cb[16];
void* i8259_cb_data[16];
uint8_t i8259_imcr;

// --

/* 8259A interrupt controller ports. */
#define i8259_INT_CTL           0x20	/* I/O port for interrupt controller */
#define i8259_INT_CTLMASK       0x21	/* setting bits in this port disables ints */
#define i8259_INT2_CTL          0xA0	/* I/O port for second interrupt controller */
#define i8259_INT2_CTLMASK      0xA1	/* setting bits in this port disables ints */

/* Magic numbers for interrupt controller. */
#define i8259_END_OF_INT        0x20	/* code used to re-enable after an interrupt */

#define CASCADE_IRQ             2       /* cascade enable for 2nd AT controller */
#define ICW1_AT                 0x11    /* edge triggered, cascade, need ICW4 */
#define ICW1_PC                 0x13    /* edge triggered, no cascade, need ICW4 */
#define ICW1_PS                 0x19    /* level triggered, cascade, need ICW4 */
#define ICW4_AT_SLAVE           0x01    /* not SFNM, not buffered, normal EOI, 8086 */
#define ICW4_AT_MASTER          0x05    /* not SFNM, not buffered, normal EOI, 8086 */
#define ICW4_PC_SLAVE           0x09    /* not SFNM, buffered, normal EOI, 8086 */
#define ICW4_PC_MASTER          0x0D    /* not SFNM, buffered, normal EOI, 8086 */
#define ICW4_AT_AEOI_SLAVE      0x03    /* not SFNM, not buffered, auto EOI, 8086 */
#define ICW4_AT_AEOI_MASTER     0x07    /* not SFNM, not buffered, auto EOI, 8086 */
#define ICW4_PC_AEOI_SLAVE      0x0B    /* not SFNM, buffered, auto EOI, 8086 */
#define ICW4_PC_AEOI_MASTER     0x0F    /* not SFNM, buffered, auto EOI, 8086 */
#define i8259_CTLMASK(irq)      ((irq) < 8 ? i8259_INT_CTLMASK : i8259_INT2_CTLMASK)

#define i8259_IMCR1             0x22
#define i8259_IMCR2             0x23

// --
void karch_i8256_init() {
    kmemset(i8259_cb, 0, sizeof(i8259_cb));
    kmemset(i8259_cb_data, 0, sizeof(i8259_cb_data));

    i8259_imcr = 1; // --> enabled initially.

    // --> setup the IDT table.
    karch_i8259_setup_idt();

    // --> program the i8259 pic.
    karch_i8259_program();
}

void karch_i8259_program() {
    cpu_out8(i8259_INT_CTL, ICW1_AT);
    cpu_out8(i8259_INT_CTLMASK, GATEVEC_PIC_MASTER);
    cpu_out8(i8259_INT_CTLMASK, (1 << CASCADE_IRQ));
      
    cpu_out8(i8259_INT_CTLMASK, ICW4_AT_MASTER);
    cpu_out8(i8259_INT_CTLMASK, ~0); // (1 << CASCADE_IRQ));

    cpu_out8(i8259_INT2_CTL, ICW1_AT);
    cpu_out8(i8259_INT2_CTLMASK, GATEVEC_PIC_SLAVE);
    cpu_out8(i8259_INT2_CTLMASK, CASCADE_IRQ);

    cpu_out8(i8259_INT2_CTLMASK, ICW4_AT_SLAVE);
    cpu_out8(i8259_INT2_CTLMASK, ~0);
}

void karch_i8259_setup_idt() {
    karch_tables_load_idt(gv_i8259_master, GATEVEC_PIC_MASTER);
    karch_tables_load_idt(gv_i8259_slave, GATEVEC_PIC_SLAVE);
}

void karch_i8259_unmask(karch_i8259_irq_t irq) {
    const uint8_t ctlmask = i8259_CTLMASK(irq);
    const uint8_t last = cpu_in8(ctlmask);

    cpu_out8(ctlmask, last & ~(1 << (irq & 0x07)));
    if (irq >= 8 && last == 0xff) {
        karch_i8259_unmask(CASCADE_IRQ);
    }
}

void karch_i8259_mask(karch_i8259_irq_t irq) {
    const uint8_t ctlmask = i8259_CTLMASK(irq);
    const uint8_t last = cpu_in8(ctlmask);
    const uint8_t newval = last | (1 << (irq & 0x07));

    cpu_out8(ctlmask, newval);
    if (irq >= 8 && newval == 0xff) {
        karch_i8259_mask(CASCADE_IRQ);
    }
}

void karch_i8259_disable() {
    cpu_out8(i8259_INT2_CTLMASK, 0xff);
    cpu_out8(i8259_INT_CTLMASK, 0xff);
    cpu_in8(i8259_INT_CTLMASK);
}

void karch_i8259_eoi(karch_i8259_irq_t n) {
    if(n < 8) {
        cpu_out8(i8259_INT_CTL, i8259_END_OF_INT);
    }
    else {
        cpu_out8(i8259_INT_CTL, i8259_END_OF_INT);
        cpu_out8(i8259_INT2_CTL, i8259_END_OF_INT);
    }
}

void karch_i8259_imcr_disable() {
    karch_i8259_disable();

    // --> disconnect i8259.
    if (0 && i8259_imcr != 0) {
        i8259_imcr = 0;
        cpu_out8(i8259_IMCR1, 0x70);
        cpu_out8(i8259_IMCR2, 0x01);
    }
}

void karch_i8259_imcr_enable() {
    if (i8259_imcr == 0) {
        cpu_out8(i8259_IMCR1, 0x70);
        cpu_out8(i8259_IMCR2, 0x00);
    }
}

uint8_t karch_i8259_get_handler(karch_i8259_irq_t n, karch_i8259_cb_t* cb, void** data) {
    if (n > 15) {
        return 0;
    }

    if (cb) {
        *cb = i8259_cb[n];
    }

    if (data) {
        *data = i8259_cb_data[n];
    }

    return 1;
}

uint8_t karch_i8259_set_handler(karch_i8259_irq_t n, karch_i8259_cb_t cb, void* data) {
    if (n > 15) {
        return 0;
    }

    i8259_cb_data[n] = data;
    i8259_cb[n] = cb;
    
    cpu_mfence();
    return 1;
}

void karch_i8259_hwint(uint32_t n, uint32_t k, karch_intr_frame_t* frame) {
    if (i8259_cb[n]) {
        karch_i8259_t e;

        e.n = n;
        e.k = k;
        e.frame = frame;
        e.data = i8259_cb_data[n];

        i8259_cb[n](&e);
    }

    // --> emit EOI to PIC master and slave.
    karch_i8259_eoi((karch_i8259_irq_t)n);
    if (!k) {
        // --> switch to user if possible.
    }
}
