#define __ARCH_X86_INTERNALS__ 1
#include <x86/smp.h>
#include <x86/klib.h>
#include <x86/env.h>

#include <x86/boot/bootinfo.h>
#include <x86/k86/tables.h>
#include <x86/k86/taskseg.h>

#include <x86/legacy/i8259.h>
#include <x86/legacy/i8253.h>

#include <x86/peripherals/acpi.h>
#include <x86/peripherals/apic.h>

#include <zeronix/kstring.h>

// --
#define BIOS_RESET_VECTOR_ADDR  0x467
#define BIOS_RESET_VECTOR       *((volatile uint32_t*) BIOS_RESET_VECTOR_ADDR)

#define RTC_INDEX               0x70
#define RTC_IO                  0x71

#define SMP_AP_ENTRY_COPY_ADDR  0x8000
#define SMP_WAIT_CPU_MDELAY     (60 * 1000)

#define SMP_READY_BITMAP_BYTES  (((MAX_CPU % 8) ? 1 : 0) + (MAX_CPU / 8))
#define SMP_BITMAP_INDEX(x)     ((x) / 8)
#define SMP_BITMAP_MASK(x)      (1 << ((x) % 8))

// -- symbols to put data for CPUs.
extern void smp_ap_entry();         // --> AP entry.
extern void* __smp_ap_id;           // --> CPU ID
extern void* __smp_ap_pt;           // --> PAGE TABLE
extern void* __smp_ap_gdt;          // --> GDT descriptor
extern void* __smp_ap_idt;          // --> IDT descriptor  
extern void* __smp_ap_gdt_tab;      // --> GDT table.
extern void* __smp_ap_idt_tab;      // --> IDT table.
extern void* __smp_entry_end;       // --> end of SMP entry.

#define smp_ap_id       *((volatile uint32_t*) &__smp_ap_id)
#define smp_ap_pt       *((volatile uint32_t*) &__smp_ap_pt)
#define smp_ap_gdt       (*((karch_desc_t*) &__smp_ap_gdt))
#define smp_ap_idt       (*((karch_desc_t*) &__smp_ap_idt))
#define smp_ap_gdt_tab   (*((karch_seg_t*) &__smp_ap_gdt_tab))
#define smp_ap_idt_tab   (*((karch_gate_t*) &__smp_ap_idt_tab))
#define smp_ap_entry_beg ((uint32_t) &smp_ap_entry)
#define smp_ap_entry_end ((uint32_t) &__smp_entry_end)

// --
uint32_t smp_bios_rstv;         // --> bios reset vector.
uint8_t smp_avail;
int16_t smp_ap_probe;           // --> probe to detect AP's boot completion.
uint8_t smp_ready_n;

// --
karch_spinlock_t smp_cpu_lock[MAX_CPU]; // --> spinlocks for each CPUs.
karch_cpuinfo_t smp_cpu_ident[MAX_CPU]; // --> SMP CPU identification.
uint8_t smp_ready_bitmap[SMP_READY_BITMAP_BYTES];

// --
void karch_smp_fill_info();
void karch_smp_start_ap();
void karch_smp_init_ap32();

uint8_t karch_smp_supported() {
    return smp_avail;
}

// --
int32_t karch_smp_init() {
    kmemset(smp_cpu_lock, 0, sizeof(smp_cpu_lock));
    kmemset(smp_cpu_ident, 0, sizeof(smp_cpu_ident));
    smp_avail = 0;
    smp_ready_n = 0;

    // --> SMP requires ACPI and APIC to work.
    if (!karch_acpi_supported() || !karch_apic_supported()) {
        karch_smp_fill_info();
        return -1;
    }
    
    // --> if no lapic exists: not supported.
    uint32_t n = karch_lapic_count();
    if (n <= 0) {
        karch_smp_fill_info();
        return -1;
    }

    // --> assign default stacktop for each task segment.
    for(uint32_t i = 0; i < n; ++i) {
        if (!karch_taskseg_setup(i, 0)) {
            break;
        }

        // --> prepare cpu lock.
        karch_spinlock_init(&smp_cpu_lock[i]);
        smp_ready_n++;
    }

    // --> set SMP available.
    smp_avail = 1;

    // --> get the lapic number of BSP CPU.
    int32_t lapic_no = karch_lapic_number();
    if (lapic_no < 0 || !karch_lapic_enable(0)) {
        smp_avail = 0;

        // --> reset interrupt tables to default k86 state.
        karch_apic_reset_idt();
        karch_smp_fill_info();
        return -1;
    }

    // --> disable i8259 icmr.
    //   : this will enable IOAPIC all.
    karch_i8259_imcr_disable();
    
    // --> load APIC's IDT and flush it..
    karch_apic_load_idt();
    karch_tables_flush_idt();

    // --
    karch_stackmark_t* sm = karch_taskseg_get_stackmark(lapic_no);

    // --> switch kernel stack to.
    switch_stack(sm, karch_smp_start_ap);

    // --> can not reach here.
    return 0;
}

uint32_t karch_smp_roundup(uint32_t n, uint32_t v) {
    if ((n % v) != 0) {
        return (n / v + 1) * v;
    }

    return n;
}

/**
 * compute relative address.
 */
#define SMP_BOOT_ADDR(x)     \
    ((((uint32_t)(x)) - ((uint32_t) &smp_ap_entry)) + SMP_AP_ENTRY_COPY_ADDR)

/**
 * prepare entry point for each APs.
 * returns 0xffffffffu if failed, otherwise, it is lowest address.
 */
uint32_t karch_smp_prepare_ap_entry() {
    const uint32_t size = smp_ap_entry_end - smp_ap_entry_beg;

    karch_table_desc_t desc;
    uint32_t lowmem = SMP_AP_ENTRY_COPY_ADDR;

    karch_tables_get_descriptors(&desc);

    //karch_desc_t* idt = karch_get_idt_ptr();
    //karch_desc_t* gdt = karch_get_gdt_ptr();
    bootinfo_t* boot = karch_k86_bootinfo();

    smp_ap_id = 0;
    smp_ap_pt = (uint32_t) boot->pagedir; // --> share BSP's page table.

    // --> address calculation was corrupted, so, I've rewrote this.
    smp_ap_gdt.base = SMP_BOOT_ADDR((uint32_t)&smp_ap_gdt_tab);
    smp_ap_idt.base = SMP_BOOT_ADDR((uint32_t)&smp_ap_idt_tab);
    smp_ap_gdt.limit = desc.gdt.limit;
    smp_ap_idt.limit = desc.idt.limit;

    kmemcpy(&smp_ap_idt_tab, (uint8_t*)desc.idt.base, desc.idt.limit + 1);
    kmemcpy(&smp_ap_gdt_tab, (uint8_t*)desc.gdt.base, desc.gdt.limit + 1);

    // --> copy ap entry fn itself.
    kmemcpy((void*) lowmem, &smp_ap_entry, size);

    return lowmem;
}

/**
 * start all APs in CPU.
 */
void karch_smp_start_ap() {
    // karch_emergency_print("SMP: starting APs...");
    uint8_t bsp_id = karch_lapic_number();

    // --> backup bios reset vector.
    smp_bios_rstv = BIOS_RESET_VECTOR;
    
    // --> set bios shutdown code to 0x0a.
    cpu_out8(RTC_INDEX, 0x0f);
    cpu_out8(RTC_IO, 0x0a);

    uint32_t ap_entry = karch_smp_prepare_ap_entry();
    if (ap_entry == 0xffffffffu) {
        // kernel panic: no memory to prepare SMP entry for each APs.
        // karch_emergency_print("fatal: no memory to prepare SMP entry for each APs.");
        while(1);
    }

    BIOS_RESET_VECTOR = ap_entry; // --> now, APs will boot here.

    // --> AP's ID pointer that points current ID value.
    uint32_t* ap_id = (uint32_t*) (ap_entry + ((uint32_t)&smp_ap_id - (uint32_t)&smp_ap_entry));
    for (uint8_t i = 0; i < smp_ready_n; ++i) {
        if (bsp_id == i) {  // --> skip boot CPU.
            continue;
        }

        // --> set the CPU ID.
        const uint8_t now_id = i;
        *ap_id = smp_ap_id = i;
        
        // --> reset the probe to wait boot completion state.
        smp_ap_probe = -1;
        cpu_mfence();

        if (!karch_lapic_send_init_ipi(now_id)) {
            // --> failed to send init command.
            continue;
        }

        if (!karch_lapic_send_startup_ipi(now_id, ap_entry)) {
            // --> failed to send startup command.
            continue;
        }

        uint32_t counter = 0; /* delay: max 1 min. */
        while (counter++ < SMP_WAIT_CPU_MDELAY) { // --> 60 seconds.
            karch_lapic_mdelay(1); 
            cpu_mfence();

            // --> CPU is ready now.
            if (smp_ap_probe == now_id) {
                break;
            }
        }
    }

    // --> restore bios reset vector here.
    BIOS_RESET_VECTOR = smp_bios_rstv;

    // --> set bios shutdown code to 0x00.
    cpu_out8(RTC_INDEX, 0x0f);
    cpu_out8(RTC_IO, 0x00);

    // --> fill SMP informations for current running CPU.
    karch_smp_fill_info();

    // TODO: run kmain on BSP.
    
    while(1);
}

void karch_smp_fill_info() {
    int32_t now_id = karch_lapic_number();
    if (now_id < 0) {
        return;
    }

    // --> identify current running BSP.
    karch_env_cpuinfo(&smp_cpu_ident[now_id]);

    // --> set mask on bitmap.
    uint32_t bit_i = SMP_BITMAP_INDEX(now_id);
    smp_ready_bitmap[bit_i] |= SMP_BITMAP_MASK(now_id);
}

uint8_t karch_smp_get_cpuinfo(uint8_t n, karch_smp_cpuinfo_t* info) {
    if (n >= karch_lapic_count()) {
        return 0;
    }

    if (!info) {
        return 1;
    }

    if (n < smp_ready_n) {
        uint32_t bit_i = SMP_BITMAP_INDEX(n);
        uint8_t state = smp_ready_bitmap[bit_i] & SMP_BITMAP_MASK(n);
        info->ready = state ? 1 : 0;
    }

    else {
        info->ready = 0;
    }
    
    //smp_ready_bitmap
    info->cpu_id = n;
    info->tss = karch_taskseg_get(n);
    info->stackmark = karch_taskseg_get_stackmark(n);
    info->lapic = karch_lapic_get(n);
    info->lock = &smp_cpu_lock[n];
    info->ident = &smp_cpu_ident[n];
    return 1;
}

uint8_t karch_smp_get_cpuinfo_current(karch_smp_cpuinfo_t* info) {
    int32_t lapic_no = karch_lapic_number();
    if (lapic_no < 0) {
        return 0;
    }

    return karch_smp_get_cpuinfo(lapic_no, info);
}

/**
 * called from `karch_smp_startup_ap32`. (<-- smp_ap_entry)
 */
void karch_smp_boot_ap32() {
    int32_t n = karch_lapic_number();
    if (n < 0) {
        // --> AP panic.
        while(1) {
            cpu_hlt();
        }

        return;
    }

    karch_stackmark_t* sm = karch_taskseg_get_stackmark(n);

    // --> reload IDT/GDT here to clear SMP boot address.
    karch_tables_flush_gdt();
    karch_tables_flush_idt();

    // --> switch stack space, then continue to `karch_smp_init_ap32`.
    switch_stack(sm, karch_smp_init_ap32);
}

/**
 * start the AP.
 */
void karch_smp_init_ap32() {
    uint32_t now_id = smp_ap_id;
    uint16_t* vga = (uint16_t*) 0xb8000;

    // --> identify current running AP.
    karch_smp_fill_info();

    // --> set completion here.
    smp_ap_probe = now_id;
    cpu_mfence();
    
    *(vga + (now_id + 1)) = ('0' + now_id) | (15 << 8);
    while(1);
}
