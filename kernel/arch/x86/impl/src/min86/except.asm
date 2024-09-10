section .text

; code segment.
SEG_KERN_CS equ (1 * 8)

extern karch_except;

global karch_except_00;
global karch_except_01;
global karch_except_02;
global karch_except_03;
global karch_except_04;
global karch_except_05;
global karch_except_06;
global karch_except_07;
global karch_except_08;
global karch_except_09;
global karch_except_10;
global karch_except_11;
global karch_except_12;
global karch_except_13;
global karch_except_14;
global karch_except_16;
global karch_except_17;
global karch_except_18;

; -----------------
; interrupt handler body.
; this calls `karch_except()` function.
; -----------------
%macro KARCH_EXCEPT_INTR_xx 1
    push ebp    ; + 4
    push ebx    ; + 4
    push eax    ; + 4
    mov ebp, esp

    ; [ebp + 16]: cs or sp / == cs: kernel mode.
    cmp dword [ebp + 16], SEG_KERN_CS
    je .kern
    ; ip, cs, flags + sp, ss are pushed.
    mov ebx, 0  ; --> user to kernel switched.

    .all:
        ; --> compute frame pointer.
        mov eax, ebp
        sub eax, 12

        ; call karch_except(uint32_t n, uint32_t k, karch_except_frame_t* frame).
        push eax        ; --> frame.
        push ebx        ; --> k

        mov eax, %1     ; --> n
        push eax

        call karch_except

        mov esp, ebp
        pop eax
        pop ebx
        pop ebp
        iret

    .kern:
        ; ip, cs, flags are pushed.
        mov ebx, 1  ; --> not switched.
        jmp .all
%endmacro

karch_except_00:
    KARCH_EXCEPT_INTR_xx 0

karch_except_01:
    KARCH_EXCEPT_INTR_xx 1

karch_except_02:
    KARCH_EXCEPT_INTR_xx 2

karch_except_03:
    KARCH_EXCEPT_INTR_xx 3

karch_except_04:
    KARCH_EXCEPT_INTR_xx 4

karch_except_05:
    KARCH_EXCEPT_INTR_xx 5

karch_except_06:
    KARCH_EXCEPT_INTR_xx 6

karch_except_07:
    KARCH_EXCEPT_INTR_xx 7

karch_except_08:
    KARCH_EXCEPT_INTR_xx 8

karch_except_09:
    KARCH_EXCEPT_INTR_xx 9

karch_except_10:
    KARCH_EXCEPT_INTR_xx 10
    
karch_except_11:
    KARCH_EXCEPT_INTR_xx 11
    
karch_except_12:
    KARCH_EXCEPT_INTR_xx 12
    
karch_except_13:
    KARCH_EXCEPT_INTR_xx 13
    
karch_except_14:
    KARCH_EXCEPT_INTR_xx 14
    
karch_except_16:
    KARCH_EXCEPT_INTR_xx 16
    
karch_except_17:
    KARCH_EXCEPT_INTR_xx 17
    
karch_except_18:
    KARCH_EXCEPT_INTR_xx 18
