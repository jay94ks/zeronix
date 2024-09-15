#include <x86/spinlock.h>
#include <x86/klib.h>

void karch_x86_spinlock_init(karch_spinlock_t* spinlock) {
    spinlock->atom = 0; // --> nobody has ownership.
}

uint8_t karch_x86_spinlock_trylock(karch_spinlock_t* spinlock) {
    uint32_t* atom = (uint32_t*) &(spinlock->atom);

    if (cpu_cmpxchg32(atom, 0, 1) == 0) {
        return 0;
    }

    return 1;
}

void karch_x86_spinlock_lock(karch_spinlock_t* spinlock) {
    while (karch_x86_spinlock_trylock(spinlock) == 0) {
        cpu_mfence();
    }
}

uint8_t karch_x86_spinlock_unlock(karch_spinlock_t* spinlock) {
    uint32_t* atom = (uint32_t*) &(spinlock->atom);

    if (cpu_cmpxchg32(atom, 1, 0) == 0) {
        return 0;
    }

    return 1;
}