#include <zeronix/types.h>


extern "C" void kmain() {
    uint16_t* vga = (uint16_t*) 0xb8000;

    *vga = ('a' | 15 << 8);

}
