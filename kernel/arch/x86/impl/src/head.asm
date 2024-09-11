section .kstack
global _karch_tss0_stack

; max cpu. --> defined at `include/zeronix/arch/x86/layout.h
MAX_CPU			equ	16

; i686 machine's page size.
I686_PAGE_SIZE	equ 4096	

_karch_stack_head:
	resb 2048			; 2K stack to call `karch_init` function.
_karch_stack_tail:
	resb (I686_PAGE_SIZE * MAX_CPU)
_karch_tss0_stack:

;=======================================================================
; Architecture Initializer.
;=======================================================================

section .text
global __karch_init
global _karch_tss0_stack

extern kmain
extern karch_init

; --> from `LD`.
extern _kinit_head
extern _kinit_tail

; here, eax will have boot info yet.
__karch_init:
    ; re-construct stack.
    mov esp, _karch_stack_tail
    mov ebp, esp

    push 0
    push eax

    call karch_init

    ; then, invoke all initializers.
	mov ebx, _kinit_head
	jmp .p2
	.p1:
		call [ebx]
		add ebx, 4

	.p2:
		cmp ebx, _kinit_tail
		jb .p1

    ; enter to kernel.
	call kmain 

	.hang:
		hlt
		jmp .hang