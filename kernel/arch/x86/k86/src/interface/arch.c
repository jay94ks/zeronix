#define __ARCH_X86_INTERNALS__ 1
#include <zeronix/arch/arch.h>
#include <x86/interface.h>
#include <x86/k86/mmap.h>
#include <x86/k86/systick.h>
#include <x86/k86/irq.h>

karch_irq_frame_t* karch_irq_get_frame_cv() {
    return (karch_irq_frame_t*) karch_irq_get_frame();
}

/**
 * exported to <zeronix/arch/arch.h>.
 */
void karch_interface(karch_t* arch) {
    if (!arch) {
        return;
    }

    arch->mem_free = &mmap_free;
    arch->mem_kern_virt = &mmap_kern_virt;
    arch->systick_freq = systick_freq;
    karch_interface_smp(&arch->smp);
    
    // --> set IRQ interfaces.
    arch->irq.count = karch_irq_count();
    arch->irq.reg = karch_irq_register;
    arch->irq.unreg = karch_irq_unregister;
    arch->irq.mask = karch_irq_mask;
    arch->irq.unmask = karch_irq_unmask;
    arch->irq.get_frame = karch_irq_get_frame_cv;
    
}