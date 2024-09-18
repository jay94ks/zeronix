#ifndef __KERNEL_ARCH_X86_INC_X86_PERIPHERALS_APIC_H__
#define __KERNEL_ARCH_X86_INC_X86_PERIPHERALS_APIC_H__

#include <x86/types.h>
#ifdef __cplusplus
extern "C" {
#endif

// code here.

/**
 * karch_x86_ioapic. 
 */
struct karch_x86_ioapic {
    uint32_t    id;
    uint32_t    addr;
    uint32_t    paddr;
    uint32_t    vaddr;
    uint32_t    pins;
    uint32_t    gsi_base;
} __packed;

/**
 * shorthand. 
 */
typedef struct karch_x86_ioapic karch_ioapic_t;

// --> forward decl.
struct karch_x86_ioapic_irq;
typedef void(* karch_ioapic_eoi_t)(uint8_t irq);

/**
 * karch_x86_ioapic_irq.
 */
struct karch_x86_ioapic_irq {
    karch_ioapic_t*     apic;
    uint32_t            pin;
    uint32_t            vector;
    karch_ioapic_eoi_t  eoi;
    uint32_t            state;  // --> 1: masked.
} __packed;

/**
 * shorthand. 
 */
typedef struct karch_x86_ioapic_irq karch_ioapic_irq_t;

/**
 * karch_lapic_info_t.
 */
typedef struct {
    uint32_t addr;
    uint32_t vaddr;
    uint32_t eoi_addr;
} karch_lapic_info_t;

/**
 * karch_lapic_t.
 */
typedef struct {
    uint32_t            ready;      // --> ready or not.
    uint8_t             id;         // --> APIC ID.
    uint8_t             bsp;        // --> indicates whether the APIC is for BSP or not.
    uint32_t            freq;       // --> CPU frequency.
    uint32_t            lapic_freq; // --> LAPIC bus frequency.
} karch_lapic_t;

/**
 * karch_lapic_irq_t.
 */
typedef enum {

    /* special interrupts. */
    LAPICIRQ_SCHEDULE   = 0xf1,
    LAPICIRQ_CPU_HALT   = 0xf2,
    LAPICIRQ_SPURIOUS   = 0xff,
    LAPICIRQ_ERROR      = 0xfe,

    /* BSP interrupts: same with SCHEDULE. */
    LAPICIRQ_TIMER      = 0xf0
} karch_lapic_irq_t;

/**
 * karch_lapic_intr_t.
 * this is a data structure for notifying interrupts.
 */
typedef struct {
    uint32_t n;
    uint32_t k;
    karch_intr_frame_t* frame;
} karch_lapic_intr_t;

/**
 * A callback type to pass interrupt execution to other.
 */
typedef void (*karch_apic_cb_t)(const karch_lapic_intr_t* i8259);

/**
 * karch_lapic_ipi_t
 */
typedef enum {
    LAPICIPI_SPECIFIC = 0,
    LAPICIPI_SELF = 1,
    LAPICIPI_BROADCAST = 2,
    LAPICIPI_EXCEPT_SELF = 3
} karch_lapic_ipi_t;

/**
 * returns whether the APIC is supported or not.
 */
uint8_t karch_apic_supported();

#ifdef __ARCH_X86_INTERNALS__
/**
 * initialize the APIC on the CPU.
*/
uint8_t karch_apic_init();
#endif

/**
 * get the current lapic id.
 * if not supported, returns negative value.
 */
int32_t karch_lapic_id();

/**
 * get the current lapic number.
 * if not supported, returns negative value.
 */
int32_t karch_lapic_number();

/**
 * get the total count of lapics in the CPU.
 */
uint32_t karch_lapic_count();

/**
 * get the Nth lapic information pointer.
 */
karch_lapic_t* karch_lapic_get(uint8_t n);

/**
 * get the current lapic.
*/
karch_lapic_t* karch_lapic_get_current();

/**
 * read an ioapic register. 
 */
uint32_t karch_ioapic_read(uint32_t iobase, uint32_t reg);

/**
 * write an ioapic register.
 */
void karch_ioapic_write(uint32_t iobase, uint32_t reg, uint32_t val);
/**
 * enable pin.
 */
void karch_ioapic_enable_pin(uint32_t ioapic_addr, uint32_t pin);

/**
 * disable pin.
 */
void karch_ioapic_disable_pin(uint32_t ioapic_addr, uint32_t pin);

/**
 * read an lapic register.
 */
uint32_t karch_lapic_read(uint32_t iobase);

/**
 * write an lapic register.
 */
void karch_lapic_write(uint32_t iobase, uint32_t val);

/**
 * get the lapic error number.
 */
uint32_t karch_lapic_error();

/**
 * get the apic interrupt handler for specified irq.
 * returns non-zero if success.
 */
uint8_t karch_apic_get_handler(uint32_t n, karch_apic_cb_t* cb);

/**
 * set the apic interrupt handler for specified irq.
 * returns non-zero if success.
 */
uint8_t karch_apic_set_handler(uint32_t n, karch_apic_cb_t cb);

/**
 * signal the end of interrupt handler to apic.
 */
uint8_t karch_lapic_eoi();

/**
 * enable the specified Nth's LAPIC.
 */
uint8_t karch_lapic_enable(uint8_t n);

/**
 * set oneshot timer that is based on Nth LAPIC.
 */
uint8_t karch_lapic_oneshot_timer(uint32_t usec);

/**
 * set periodic timer that is based on Nth LAPIC.
 */
uint8_t karch_lapic_periodic_timer(uint32_t freq);

/**
 * stop the LAPIC timer.
 * this is available after lapic initialized.
 */
void karch_lapic_stop_timer();

/**
 * micro-delay based on LAPIC timer.
 */
void karch_lapic_udelay(uint32_t n);

/**
 * milli-delay based on LAPIC timer.
 */
void karch_lapic_mdelay(uint32_t ms);

/**
 * result codes for `wait_flag_*` functions.
 * karch_lapic_wr_t: wait result type.
*/
typedef enum {
    LAPICWR_SUCCESS = 0,
    LAPICWR_EXHAUSTED = 1,
    LAPICWR_ERROR = 2,
} karch_lapic_wr_t;

/**
 * wait for LAPIC flag to be set until `tries`.
 * this is available after lapic initialized.
 * ----
 * returns: karch_lapic_waitresult_t.
 */
karch_lapic_wr_t karch_lapic_wait_flag_set(uint32_t addr, uint32_t flag, uint32_t tries);

/**
 * wait for LAPIC flag to be cleared until `tries`.
 * this is available after lapic initialized.
 * ----
 * returns: karch_lapic_waitresult_t.
 */
karch_lapic_wr_t karch_lapic_wait_flag_clear(uint32_t addr, uint32_t flag, uint32_t tries);

#ifdef __ARCH_X86_INTERNALS__
/**
 * load IDT entities for APIC interrupts.
 */
void karch_apic_load_idt();

/**
 * reset IDT entities to unload APIC interrupts.
 */
void karch_apic_reset_idt();

/**
 * send init ipl to specified Nth lapic. 
 * returns non-zero value if succeed.
 */
uint8_t karch_lapic_send_init_ipi(uint8_t n);

/**
 * send startup ipi to specified Nth lapic.
 * returns non-zero value if succeed.
 */
uint8_t karch_lapic_send_startup_ipi(uint8_t n, uint32_t entry_point);
#endif

/**
 * test whether the APIC is pending to deliver ipi or not.
 * returns non-zero value if pending.
 */
uint8_t karch_lapic_pending_ipi();

/**
 * emit an IPI to specified LAPIC.
 * returns non-zero value if succeed.
 */
uint8_t karch_lapic_emit_ipi(uint8_t vector, uint32_t n, karch_lapic_ipi_t how);

/**
 * enable IO-APIC all.
 */
void karch_ioapic_enable_all();

/**
 * set an IRQ redirector on IO-APIC.
 */
void karch_ioapic_set_irq(uint8_t irq);

/**
 * disable an IRQ on IO-APIC.
 */
void karch_ioapic_disable_irq(uint8_t irq);

/**
 * enable an IRQ on IO-APIC.
 */
void karch_ioapic_enable_irq(uint8_t irq);

// --
#ifndef __NO_EXTERN_LAPIC_VARS__
extern karch_lapic_info_t lapic_info;
#endif
#define LAPIC_ADDR                  lapic_info.addr

#define MAX_IOAPIC                  32
#define MAX_IOAPIC_IRQ              64

#define APIC_ENABLE		            0x100
#define APIC_FOCUS_DISABLED	        (1 << 9)
#define APIC_SIV		            0xFF

#define APIC_TDCR_2                 0x00
#define APIC_TDCR_4	                0x01
#define APIC_TDCR_8	                0x02
#define APIC_TDCR_16	            0x03
#define APIC_TDCR_32	            0x08
#define APIC_TDCR_64	            0x09
#define APIC_TDCR_128	            0x0a
#define APIC_TDCR_1	                0x0b

#define APIC_LVTT_VECTOR_MASK	    0x000000FF
#define APIC_LVTT_DS_PENDING	    (1 << 12)
#define APIC_LVTT_MASK		        (1 << 16)
#define APIC_LVTT_TM		        (1 << 17)

#define APIC_LVT_IIPP_MASK	        0x00002000
#define APIC_LVT_IIPP_AH	        0x00002000
#define APIC_LVT_IIPP_AL	        0x00000000

#define IOAPIC_REGSEL		        0x0
#define IOAPIC_RW		            0x10

#define APIC_ICR_DM_MASK		    0x00000700
#define APIC_ICR_VECTOR			    APIC_LVTT_VECTOR_MASK
#define APIC_ICR_DM_FIXED		    (0 << 8)
#define APIC_ICR_DM_LOWEST_PRIORITY	(1 << 8)
#define APIC_ICR_DM_SMI			    (2 << 8)
#define APIC_ICR_DM_RESERVED		(3 << 8)
#define APIC_ICR_DM_NMI			    (4 << 8)
#define APIC_ICR_DM_INIT		    (5 << 8)
#define APIC_ICR_DM_STARTUP		    (6 << 8)
#define APIC_ICR_DM_EXTINT		    (7 << 8)

#define APIC_ICR_DM_PHYSICAL		(0 << 11)
#define APIC_ICR_DM_LOGICAL		    (1 << 11)

#define APIC_ICR_DELIVERY_PENDING	(1 << 12)

#define APIC_ICR_INT_POLARITY		(1 << 13)

#define APIC_ICR_LEVEL_ASSERT		(1 << 14)
#define APIC_ICR_LEVEL_DEASSERT		(0 << 14)

#define APIC_ICR_TRIGGER		    (1 << 15)

#define APIC_ICR_INT_MASK		    (1 << 16)

#define APIC_ICR_DEST_FIELD		    (0 << 18)
#define APIC_ICR_DEST_SELF		    (1 << 18)
#define APIC_ICR_DEST_ALL		    (2 << 18)
#define APIC_ICR_DEST_ALL_BUT_SELF	(3 << 18)

#define LOCAL_APIC_DEF_ADDR	        0xfee00000 /* default local apic address */
#define IO_APIC_DEF_ADDR	        0xfec00000 /* default i/o apic address */

#define LAPIC_ID	                (LAPIC_ADDR + 0x020)
#define LAPIC_VERSION	            (LAPIC_ADDR + 0x030)
#define LAPIC_TPR	                (LAPIC_ADDR + 0x080)
#define LAPIC_EOI	                (LAPIC_ADDR + 0x0b0)
#define LAPIC_LDR	                (LAPIC_ADDR + 0x0d0)
#define LAPIC_DFR	                (LAPIC_ADDR + 0x0e0)
#define LAPIC_SIVR	                (LAPIC_ADDR + 0x0f0)
#define LAPIC_ISR	                (LAPIC_ADDR + 0x100)
#define LAPIC_TMR	                (LAPIC_ADDR + 0x180)
#define LAPIC_IRR	                (LAPIC_ADDR + 0x200)
#define LAPIC_ESR	                (LAPIC_ADDR + 0x280)
#define LAPIC_ICR1	                (LAPIC_ADDR + 0x300)
#define LAPIC_ICR2	                (LAPIC_ADDR + 0x310)
#define LAPIC_LVTTR	                (LAPIC_ADDR + 0x320)
#define LAPIC_LVTTMR	            (LAPIC_ADDR + 0x330)
#define LAPIC_LVTPCR	            (LAPIC_ADDR + 0x340)
#define LAPIC_LINT0	                (LAPIC_ADDR + 0x350)
#define LAPIC_LINT1	                (LAPIC_ADDR + 0x360)
#define LAPIC_LVTER	                (LAPIC_ADDR + 0x370)
#define LAPIC_TIMER_ICR	            (LAPIC_ADDR + 0x380)
#define LAPIC_TIMER_CCR	            (LAPIC_ADDR + 0x390)
#define LAPIC_TIMER_DCR	            (LAPIC_ADDR + 0x3e0)

#define IOAPIC_ID			        0x0
#define IOAPIC_VERSION			    0x1
#define IOAPIC_ARB			        0x2
#define IOAPIC_REDIR_TABLE		    0x10

#define APIC_TIMER_INT_VECTOR		0xf0
#define APIC_SMP_SCHED_PROC_VECTOR	0xf1
#define APIC_SMP_CPU_HALT_VECTOR	0xf2
#define APIC_ERROR_INT_VECTOR		0xfe
#define APIC_SPURIOUS_INT_VECTOR	0xff

#ifdef __cplusplus
}
#endif
#endif