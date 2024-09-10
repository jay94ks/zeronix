#ifndef __ZERONIX_ARCH_X86_LEGACY_PAGE_H__
#define __ZERONIX_ARCH_X86_LEGACY_PAGE_H__

#include <zeronix/types.h>
#include <zeronix/boot.h>

/**
 * copy page tables to the higher half region.
 */
void karch_init_page(kbootinfo_t* info);



#endif