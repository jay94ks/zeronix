#ifndef __ZERONIX_ARCH_X86_SMP_APIC_H__
#define __ZERONIX_ARCH_X86_SMP_APIC_H__

#include <zeronix/types.h>

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
typedef void(* karch_ioapic_eio_t)(struct karch_x86_ioapic_irq*);

/**
 * karch_x86_ioapic_irq.
 */
struct karch_x86_ioapic_irq {
    karch_ioapic_t*     apic;
    uint32_t            pin;
    uint32_t            vector;
    karch_ioapic_eio_t  eoi;
    uint32_t            state;
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
    uint32_t bsp_cpu_id;
    uint32_t bsp_lapic_id;
} karch_lapic_info_t;

// --

/**
 * defined at /arch/x86/impl/smp/apic.c. 
 */
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

#endif