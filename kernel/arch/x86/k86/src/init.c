#define __ARCH_X86_INTERNALS__
#include <x86/k86.h>
#include <x86/smp.h>
#include <x86/klib.h>
#include <x86/k86/cpulocal.h>

// --
void karch_emergency_print(const char* msg);
void karch_enter_kmain();

// --> defined at head.asm.
extern void jump_to_kmain();

// --
void karch_init(bootinfo_t* info) {
    // --> initialize the early x86 environment.
    karch_k86_init(info);

    // --> initialize SMP and if it has fatal error.
    if (karch_smp_init(karch_enter_kmain) == 0) {
        cpu_cli();

        // kernel panic: unreachable if SMP successfully initialized.
        karch_emergency_print(
            "fatal: the CPU reached to impossible point in SMP initialization.");

        while(1);
    }

    // --> if reached to here, 
    //   : the SMP init failed due to not supported hardware.
    karch_cpulocals_init();
    karch_enter_kmain();
}

/**
 * enter to `kmain`.
*/
void karch_enter_kmain() {
    uint8_t now_id = karch_lapic_number();
    karch_stackmark_t* sm 
        = karch_taskseg_get_stackmark(now_id);

    switch_stack(sm, jump_to_kmain);
}

void karch_emergency_print(const char* msg) {
    uint16_t* vba = (uint16_t*) 0xb8000;
    const char* msg_ = msg;

    while(*msg_) {
        *vba++ = (*msg_++) | (15 << 8);
    }

    *vba++ = '.' | (15 << 8);
    *vba++ = (15 << 8);
}
