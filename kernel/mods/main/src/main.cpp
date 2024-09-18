#include <zeronix/types.h>
#include <zeronix/arch/arch.h>
#include <zeronix/arch/irq.h>

karch_t arch;
uint32_t n;

karch_irq_t systick;

void onSysTick(karch_irq_t* irq);

extern "C" void kmain() {
    uint16_t* vga = (uint16_t*) 0xb8000;

    *vga = ('a' | 15 << 8);
    n = 0;

    karch_interface(&arch);

    systick.handler = onSysTick;
    arch.irq.reg(IRQN_SYSTICK, &systick);
}

void onSysTick(karch_irq_t* irq) {
    uint16_t* vga = (uint16_t*) 0xb8000;

    char ch = 'a' + (n++ % ('z' - 'a'));

    vga[1] = (ch | 15 << 8);
}