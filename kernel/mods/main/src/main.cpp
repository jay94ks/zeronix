#include <zeronix/types.h>
#include <zeronix/arch/arch.h>

karch_t arch;
uint32_t n;

void onSysTick();

extern "C" void kmain() {
    uint16_t* vga = (uint16_t*) 0xb8000;

    *vga = ('a' | 15 << 8);
    n = 0;

    karch_interface(&arch);

    arch.set_systick(onSysTick);
}

void onSysTick() {
    uint16_t* vga = (uint16_t*) 0xb8000;

    char ch = 'a' + (n++ % ('z' - 'a'));

    vga[1] = (ch | 15 << 8);
}