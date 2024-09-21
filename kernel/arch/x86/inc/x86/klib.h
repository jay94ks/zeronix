#ifndef __KERNEL_ARCH_X86_INC_X86_KLIB_H__
#define __KERNEL_ARCH_X86_INC_X86_KLIB_H__

#include <zeronix/types.h>
#ifdef __cplusplus
extern "C" {
#endif

// --> klib.asm
extern uint32_t read_cr0();
extern uint32_t read_cr3();
extern uint32_t read_cr4();

extern void write_cr0(uint32_t cr0); 
extern void write_cr3(uint32_t cr3); 
extern void write_cr4(uint32_t cr4); 

extern void read_cpuid(uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);

extern void read_msr(uint32_t reg, uint32_t* high, uint32_t* low);
extern void write_msr(uint32_t reg, uint32_t high, uint32_t low);

extern void load_gdt(void*);
extern void load_idt(void*);
extern void load_ldt(uint32_t);
extern void load_tr(uint32_t);
extern void store_gdt(void*);
extern void store_idt(void*);
extern uint32_t store_tr();

extern void load_kern_cs();
extern void load_ds(uint32_t ds);
extern void load_es(uint32_t ds);
extern void load_fs(uint32_t ds);
extern void load_gs(uint32_t ds);
extern void load_ss(uint32_t ds);

extern void switch_stack(void* esp, void (* continuation)(void));

extern void cpu_hlt();
extern void cpu_cli();
extern void cpu_sti();
extern void cpu_stihlt();

extern uint32_t cpu_in8(uint32_t port);
extern uint32_t cpu_in16(uint32_t port);
extern uint32_t cpu_in32(uint32_t port);

extern void cpu_out8(uint32_t port, uint32_t data);
extern void cpu_out16(uint32_t port, uint32_t data);
extern void cpu_out32(uint32_t port, uint32_t data);

// --> windows interlocked style cmpxchg.
extern uint32_t cpu_cmpxchg32(uint32_t* ptr, uint32_t exp, uint32_t val); 
extern void cpu_read_tsc(uint32_t* hi, uint32_t* lo);
extern void cpu_mfence();
extern void cpu_pause();

extern void cpu_nop();

#ifdef __cplusplus
}
#endif
#endif