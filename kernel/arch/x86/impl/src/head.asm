section .kstack
_karch_stack_head:
	resb 16384	; 1K stack to call `kboot` function.
_karch_stack_tail:
	resb 16384
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