#include <zeronix/types.h>
#include <zeronix/boot.h>
#include <zeronix/kcrt/string.h>

// --
kbootinfo_t kinfo;

void karch_init(kbootinfo_t* info) {
    kmemcpy(&kinfo, info, sizeof(kinfo));
}