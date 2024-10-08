section .kstack
_karch_stack_head:
	resb 2048			; 2K stack to call `karch_init` function.
_karch_stack_tail:

;=======================================================================
; Architecture Initializer.
;=======================================================================

section .text
global __karch_init
global jump_to_kmain

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
	jmp jump_to_kmain

jump_to_kmain:
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