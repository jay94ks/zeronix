#define __ARCH_X86_INTERNALS__ 1
#include <zeronix/arch/arch.h>
#include <x86/interface.h>
#include <x86/k86/mmap.h>
#include <x86/k86/systick.h>

/**
 * exported to <zeronix/arch/arch.h>.
 */
void karch_interface(karch_t* arch) {
    if (!arch) {
        return;
    }

    arch->mem_free = &mmap_free;
    arch->mem_kern_virt = &mmap_kern_virt;

    karch_interface_smp(&arch->smp);
    
    arch->systick_freq = systick_freq;
    arch->set_systick = karch_systick_set_handler;
    
}