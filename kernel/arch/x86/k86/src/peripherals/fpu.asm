section .text

global cpu_fninit
global cpu_fnstsw
global cpu_fnstcw
global cpu_fnsave
global cpu_fxsave

; --> this is used to trace FPU error interrupts.
;   : keep this order to expose exact symbol locations to C code.
global __fpu_error_eip_start
global cpu_fxrstor
global cpu_frstor
global __fpu_error_eip_end
global __fpu_error_eip_patch

cpu_fninit:
    fninit
    ret

cpu_fnstsw:
    xor eax, eax
    fnstsw ax
    ret

cpu_fnstcw:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    fnstcw [eax]

    mov esp, ebp
    pop ebp
    ret
    
cpu_fnsave:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    fnsave [eax]
    fwait

    mov esp, ebp
    pop ebp
    ret
    
cpu_fxsave:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    fxsave [eax]

    mov esp, ebp
    pop ebp
    ret

__fpu_error_eip_start:
cpu_fxrstor:
    mov eax, [esp + 4]
    fxrstor [eax]

    .end:
        mov eax, 1
        ret

cpu_frstor:
    mov eax, [esp + 4]
    frstor [eax]
    .end:
        mov eax, 1
        ret

__fpu_error_eip_patch:
    xor eax, eax
    ret

__fpu_error_eip_end:
