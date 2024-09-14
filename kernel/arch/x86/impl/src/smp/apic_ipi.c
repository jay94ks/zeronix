#define __ARCH_X86_INTERNALS__  1

#include "apic.h"
#include "apic_ipi.h"

#include <zeronix/kcrt/string.h>
#include <zeronix/arch/x86/cpuinfo.h>
#include <zeronix/arch/x86/klib.h>
#include <zeronix/arch/x86/spinlock.h>
#include <zeronix/arch/x86/cpu.h>
#include <zeronix/arch/x86/layout.h>

#define RTC_INDEX   0x70
#define RTC_IO      0x71

// --
void karch_lapic_set_ipi(uint32_t apic_id, uint32_t mode);

// --
uint8_t karch_lapic_send_init_ipi(uint8_t cpu_id) {
    karch_cpu_t* cpu = karch_get_cpu(cpu_id);
    if (!cpu) {
        return 0;
    }

    karch_lapic_wr_t wr;
    uint8_t apic_id = cpu->apic_id;
    
    // --> set bios shutdown code to 0x0a.
    cpu_out8(RTC_INDEX, 0x0f);
    cpu_out8(RTC_IO, 0x0a);

    // --> clear error state register.
    karch_lapic_error();

    // --> init ipl, no shorthand, dest mode: phy.
    karch_lapic_set_ipi(apic_id, APIC_ICR_DM_INIT | APIC_ICR_LEVEL_ASSERT);
    
    // --> wait for pending flag to be cleared.
    wr = karch_lapic_wait_flag_clear(LAPIC_ICR1, APIC_ICR_DELIVERY_PENDING, 1000);
    if (wr != LAPICWR_SUCCESS) {
        return 0; // --> skip current CPU.
    }

    // --> clear error state register again.
    karch_lapic_error();

    // --> de-init ipl, no shorthand, dest mode: phy.
    karch_lapic_set_ipi(apic_id, APIC_ICR_DEST_ALL);

    // --> wait for pending flag to be cleared.
    wr = karch_lapic_wait_flag_clear(LAPIC_ICR1, APIC_ICR_DELIVERY_PENDING, 1000);
    if (wr != LAPICWR_SUCCESS) {
        return 0; // --> skip current CPU.
    }
    
    // --> clear error state register again.
    karch_lapic_error();

    // --> sleep 10 millisec.
	karch_lapic_mdelay(10);
    return 1;
}

void karch_lapic_set_ipi(uint32_t apic_id, uint32_t mode) {
    // --> write ICR2: set the target PE.
    uint32_t icr2 = karch_lapic_read(LAPIC_ICR2);
    karch_lapic_write(LAPIC_ICR2, (icr2 & 0xffffff) | ((apic_id & 0xff) << 24));

    // --> write ICR1.
    uint32_t icr1 = karch_lapic_read(LAPIC_ICR1);
    karch_lapic_write(LAPIC_ICR1, (icr1 & 0xfff32000) | mode);
    
    // --> delay 200 usec.
	karch_lapic_udelay(200);
}

uint8_t karch_lapic_send_startup_ipi(uint8_t cpu_id, uint32_t entry_point) {
    karch_cpu_t* cpu = karch_get_cpu(cpu_id);
    if (!cpu) {
        return 0;
    }

    const uint32_t refined_ep = (entry_point >> 12) & 0xff;
    karch_lapic_wr_t wr;

    // --> send SIPI-SIPI sequence.
    for (uint8_t i = 0; i < 2; ++i) {
        // --> clear the error status.
        karch_lapic_error();

        // --> send SIPI.
        karch_lapic_set_ipi(cpu->apic_id,
            APIC_ICR_LEVEL_ASSERT | APIC_ICR_DM_STARTUP |
            refined_ep);

        // --> wait for pending flag to be cleared.
        wr = karch_lapic_wait_flag_clear(LAPIC_ICR1, APIC_ICR_DELIVERY_PENDING, 1000);
        if (wr != LAPICWR_SUCCESS) {
            return 0; // --> skip current CPU.
        }
    }

	return 1;
}