/**
 * This file is to include architecture specific headers.
 */

#if defined(__ARCH_X86__) && __ARCH_X86__
#include "arch/x86/bootenv.h"
#include "arch/x86/spinlock.h"
#endif

/**
 * here, all arch provides (must):
 *   karch_atom_t.
 *   karch_spinlock_t.
 * ...
 */