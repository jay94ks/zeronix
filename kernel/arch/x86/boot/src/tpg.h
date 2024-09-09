#ifndef __X86_BOOT_TPG_H__
#define __X86_BOOT_TPG_H__

#include <zeronix/types.h>

extern uint32_t tpg_read_cr0();
extern uint32_t tpg_read_cr4();

extern void tpg_write_cr0(uint32_t cr0); 
extern void tpg_write_cr3(uint32_t cr3); 
extern void tpg_write_cr4(uint32_t cr4); 

extern void tpg_cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);

#endif