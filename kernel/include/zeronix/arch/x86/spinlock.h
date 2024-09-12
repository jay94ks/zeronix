#ifndef __KERNEL_INCLUDE_ZERONIX_ARCH_X86_SPINLOCK_H__
#define __KERNEL_INCLUDE_ZERONIX_ARCH_X86_SPINLOCK_H__

#include <zeronix/types.h>

// --> atomic type.
typedef uint32_t karch_atom_t;

/**
 * spinlock.
 */
typedef struct {
    karch_atom_t atom;
} karch_spinlock_t;

/**
 * initialize a new spinlock.
 */
void karch_x86_spinlock_init(karch_spinlock_t* spinlock);
#define karch_spinlock_init karch_x86_spinlock_init

/**
 * try to get ownership for the spinlock. 
 * returns non-zero value if succeed.
 */
uint8_t karch_x86_spinlock_trylock(karch_spinlock_t* spinlock);
#define karch_spinlock_trylock karch_x86_spinlock_trylock

/**
 * get ownership for the spinlock.
 * this only returns if succeed. 
 */
void karch_x86_spinlock_lock(karch_spinlock_t* spinlock);
#define karch_spinlock_lock karch_x86_spinlock_lock

/**
 * return ownership of the spinlock.
 * returns non-zero value if succeed.
 * otherwise, no ownership took.
 */
uint8_t karch_x86_spinlock_unlock(karch_spinlock_t* spinlock);
#define karch_spinlock_unlock karch_x86_spinlock_unlock

#endif