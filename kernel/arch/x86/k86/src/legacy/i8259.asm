section .text

; code segment.
SEG_KERN_CS equ (1 * 8)

global karch_i8259_hwint_00;
global karch_i8259_hwint_01;
global karch_i8259_hwint_02;
global karch_i8259_hwint_03;
global karch_i8259_hwint_04;
global karch_i8259_hwint_05;
global karch_i8259_hwint_06;
global karch_i8259_hwint_07;
global karch_i8259_hwint_08;
global karch_i8259_hwint_09;
global karch_i8259_hwint_10;
global karch_i8259_hwint_11;
global karch_i8259_hwint_12;
global karch_i8259_hwint_13;
global karch_i8259_hwint_14;
global karch_i8259_hwint_15;
extern karch_i8259_hwint;

; -----------------
; interrupt handler body.
; this calls `karch_i8259_hwint()` function.
; -----------------
%macro KARCH_i8259_HWINT_xx 1
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

        ; call karch_i8259_hwint(uint32_t n, uint32_t k, karch_intr_frame_t* frame).
        push eax        ; --> frame.
        push ebx        ; --> k

        mov eax, %1     ; --> n
        push eax

        call karch_i8259_hwint

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

karch_i8259_hwint_00:
    KARCH_i8259_HWINT_xx(0)

karch_i8259_hwint_01:
    KARCH_i8259_HWINT_xx(1)

karch_i8259_hwint_02:
    KARCH_i8259_HWINT_xx(2)

karch_i8259_hwint_03:
    KARCH_i8259_HWINT_xx(3)

karch_i8259_hwint_04:
    KARCH_i8259_HWINT_xx(4)

karch_i8259_hwint_05:
    KARCH_i8259_HWINT_xx(5)

karch_i8259_hwint_06:
    KARCH_i8259_HWINT_xx(6)

karch_i8259_hwint_07:
    KARCH_i8259_HWINT_xx(7)

karch_i8259_hwint_08:
    KARCH_i8259_HWINT_xx(8)

karch_i8259_hwint_09:
    KARCH_i8259_HWINT_xx(9)

karch_i8259_hwint_10:
    KARCH_i8259_HWINT_xx(10)

karch_i8259_hwint_11:
    KARCH_i8259_HWINT_xx(11)

karch_i8259_hwint_12:
    KARCH_i8259_HWINT_xx(12)

karch_i8259_hwint_13:
    KARCH_i8259_HWINT_xx(13)

karch_i8259_hwint_14:
    KARCH_i8259_HWINT_xx(14)

karch_i8259_hwint_15:
    KARCH_i8259_HWINT_xx(15)
