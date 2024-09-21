
TRAPCODE_HWINT      equ 0
TRAPCODE_APIC_HWINT equ 1

TRAPCODE_SWINT      equ 2
TRAPCODE_APIC_SWINT equ 3

; stack mark has three members: prev (8), next (4), cpu (0)
TASK_CUR_PROC_PTR   equ 28

KERN_CR3            equ 0x6000

; --------------------------------
; `karch_task_t` field map:
;
; type: 0.
; cpu : 4.
; trap: 6.
; rsv : 7
; tss : n
; fpu : don't care.

; --------------------------------
; store task states.
; this shouldn't be placed if under kernel.
; --------------------------------
%macro STORE_REGS 0
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi

    pushf   ; push eflags

    push es
    push ds
    push ss
    push fs
    push gs

    mov eax, cr3
    push eax

    ; --> load kernel's page directory.
    cmp eax, KERN_CR3
    je .pass

    ; --> this will be restored when `RESTORE_REGS` get called.
    mov eax, KERN_CR3
    mov cr3, eax

    .pass:
%endmacro

%macro RESTORE_REGS 0
    pop ebx         ; pop CR3
    mov eax, cr3    ; get CR3
    cmp eax, ebx    ; compare two.
    jne .load_cr3    ; different, load new.
    je .fin

    .load_cr3:
        mov eax, ebx
        mov cr3, eax

    .fin:
        pop gs
        pop fs
        add esp, 4
        pop ds
        pop es
        
        popf    ; pop eflags.

        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
%endmacro