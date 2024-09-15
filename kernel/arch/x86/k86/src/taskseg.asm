section .kstack
global _karch_tss0_stack

; max cpu. --> defined at `x86/min86/tables.h`.
MAX_CPU			equ	16

; i686 machine's page size.
I686_PAGE_SIZE	equ 4096	

_karch_tss0_stack_head:
	resb (I686_PAGE_SIZE * MAX_CPU)
_karch_tss0_stack: