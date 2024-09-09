MBALIGN     equ  0x00000001
MEMINFO     equ  0x00000002
FLAGS       equ  MBALIGN | MEMINFO
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

;=======================================================================
; SPECIAL SECTION : this will be placed on kernel's begin.
;=======================================================================
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd 0

;=======================================================================
; Kernel Stack.
;=======================================================================
section .kstack
_kstack_head:
	resb 1024	; 1K stack to call `kboot` function.
_kstack_tail:

;=======================================================================
; Kernel entry point.
;=======================================================================
section .text

global _start
extern kboot
extern __karch_init

_start:
    ; clear interrupt masks.
    cli

    ; then, configure the kernel stack.
	mov esp, _kstack_tail
	mov ebp, esp

    ; run boot code for architecture specific.
	push ebx	; --> multiboot info.
	push eax	; --> multiboot magic number.
	
    call kboot
	add esp, 8	; --> pop ebx, eax.

    ; pass to architecture specific implementation.
	jmp __karch_init