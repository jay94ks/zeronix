/**
 * This file is to include architecture specific headers.
 */

#if defined(__ARCH_X86__) && __ARCH_X86__
#include "arch/x86/multiboot.h"
#include "arch/x86/bootenv.h"
#include "arch/x86/i686.h"
#include "arch/x86/types.h"
#include "arch/x86/cpuinfo.h"
#include "arch/x86/klib.h"
#include "arch/x86/acpi.h"
#endif