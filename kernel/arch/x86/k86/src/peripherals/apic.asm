section .text

; code segment.
SEG_KERN_CS equ (1 * 8)

; --> defined at apic_idt.c.
extern karch_apic_hwint;
extern karch_apic_zbint;

; usage: KARCH_APIC_HWINT_xx [IRQN]
%macro KARCH_APIC_HWINT_xx 1
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

        ; call karch_apic_hwint(uint32_t n, uint32_t k, karch_intr_frame_t* frame).
        push eax        ; --> frame.
        push ebx        ; --> k

        mov eax, %1     ; --> n
        push eax

        call karch_apic_hwint

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

; usage: KARCH_APIC_INTR [IRQN]
%macro KARCH_APIC_INTR  1
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

        ; call karch_apic_hwint(uint32_t k, karch_intr_frame_t* frame).
        push eax        ; --> frame.
        push ebx        ; --> k
        
        mov eax, %1
        push eax

        call karch_apic_zbint

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

; 0 ~ 63.
global karch_apic_hwint_00;
global karch_apic_hwint_01;
global karch_apic_hwint_02;
global karch_apic_hwint_03;
global karch_apic_hwint_04;
global karch_apic_hwint_05;
global karch_apic_hwint_06;
global karch_apic_hwint_07;
global karch_apic_hwint_08;
global karch_apic_hwint_09;

global karch_apic_hwint_10;
global karch_apic_hwint_11;
global karch_apic_hwint_12;
global karch_apic_hwint_13;
global karch_apic_hwint_14;
global karch_apic_hwint_15;
global karch_apic_hwint_16;
global karch_apic_hwint_17;
global karch_apic_hwint_18;
global karch_apic_hwint_19;

global karch_apic_hwint_20;
global karch_apic_hwint_21;
global karch_apic_hwint_22;
global karch_apic_hwint_23;
global karch_apic_hwint_24;
global karch_apic_hwint_25;
global karch_apic_hwint_26;
global karch_apic_hwint_27;
global karch_apic_hwint_28;
global karch_apic_hwint_29;

global karch_apic_hwint_30;
global karch_apic_hwint_31;
global karch_apic_hwint_32;
global karch_apic_hwint_33;
global karch_apic_hwint_34;
global karch_apic_hwint_35;
global karch_apic_hwint_36;
global karch_apic_hwint_37;
global karch_apic_hwint_38;
global karch_apic_hwint_39;

global karch_apic_hwint_40;
global karch_apic_hwint_41;
global karch_apic_hwint_42;
global karch_apic_hwint_43;
global karch_apic_hwint_44;
global karch_apic_hwint_45;
global karch_apic_hwint_46;
global karch_apic_hwint_47;
global karch_apic_hwint_48;
global karch_apic_hwint_49;

global karch_apic_hwint_50;
global karch_apic_hwint_51;
global karch_apic_hwint_52;
global karch_apic_hwint_53;
global karch_apic_hwint_54;
global karch_apic_hwint_55;
global karch_apic_hwint_56;
global karch_apic_hwint_57;
global karch_apic_hwint_58;
global karch_apic_hwint_59;

global karch_apic_hwint_60;
global karch_apic_hwint_61;
global karch_apic_hwint_62;
global karch_apic_hwint_63;

global karch_apic_zbint_f0;
global karch_apic_zbint_f1;
global karch_apic_zbint_f2;
global karch_apic_zbint_fe;
global karch_apic_zbint_ff;

karch_apic_hwint_00:
    KARCH_APIC_HWINT_xx 0
    
karch_apic_hwint_01:
    KARCH_APIC_HWINT_xx 1
    
karch_apic_hwint_02:
    KARCH_APIC_HWINT_xx 2
    
karch_apic_hwint_03:
    KARCH_APIC_HWINT_xx 3
    
karch_apic_hwint_04:
    KARCH_APIC_HWINT_xx 4
    
karch_apic_hwint_05:
    KARCH_APIC_HWINT_xx 5
    
karch_apic_hwint_06:
    KARCH_APIC_HWINT_xx 6
    
karch_apic_hwint_07:
    KARCH_APIC_HWINT_xx 7
    
karch_apic_hwint_08:
    KARCH_APIC_HWINT_xx 8

karch_apic_hwint_09:
    KARCH_APIC_HWINT_xx 9
    
    
karch_apic_hwint_10:
    KARCH_APIC_HWINT_xx 10
    
karch_apic_hwint_11:
    KARCH_APIC_HWINT_xx 11
    
karch_apic_hwint_12:
    KARCH_APIC_HWINT_xx 12
    
karch_apic_hwint_13:
    KARCH_APIC_HWINT_xx 13
    
karch_apic_hwint_14:
    KARCH_APIC_HWINT_xx 14
    
karch_apic_hwint_15:
    KARCH_APIC_HWINT_xx 15
    
karch_apic_hwint_16:
    KARCH_APIC_HWINT_xx 16
    
karch_apic_hwint_17:
    KARCH_APIC_HWINT_xx 17
    
karch_apic_hwint_18:
    KARCH_APIC_HWINT_xx 18

karch_apic_hwint_19:
    KARCH_APIC_HWINT_xx 19
    
    
karch_apic_hwint_20:
    KARCH_APIC_HWINT_xx 20
    
karch_apic_hwint_21:
    KARCH_APIC_HWINT_xx 21
    
karch_apic_hwint_22:
    KARCH_APIC_HWINT_xx 22
    
karch_apic_hwint_23:
    KARCH_APIC_HWINT_xx 23
    
karch_apic_hwint_24:
    KARCH_APIC_HWINT_xx 24
    
karch_apic_hwint_25:
    KARCH_APIC_HWINT_xx 25
    
karch_apic_hwint_26:
    KARCH_APIC_HWINT_xx 26
    
karch_apic_hwint_27:
    KARCH_APIC_HWINT_xx 27
    
karch_apic_hwint_28:
    KARCH_APIC_HWINT_xx 28

karch_apic_hwint_29:
    KARCH_APIC_HWINT_xx 29
    
    
karch_apic_hwint_30:
    KARCH_APIC_HWINT_xx 30
    
karch_apic_hwint_31:
    KARCH_APIC_HWINT_xx 31
    
karch_apic_hwint_32:
    KARCH_APIC_HWINT_xx 32
    
karch_apic_hwint_33:
    KARCH_APIC_HWINT_xx 33
    
karch_apic_hwint_34:
    KARCH_APIC_HWINT_xx 34
    
karch_apic_hwint_35:
    KARCH_APIC_HWINT_xx 35
    
karch_apic_hwint_36:
    KARCH_APIC_HWINT_xx 36
    
karch_apic_hwint_37:
    KARCH_APIC_HWINT_xx 37
    
karch_apic_hwint_38:
    KARCH_APIC_HWINT_xx 38

karch_apic_hwint_39:
    KARCH_APIC_HWINT_xx 39
    
    
karch_apic_hwint_40:
    KARCH_APIC_HWINT_xx 40
    
karch_apic_hwint_41:
    KARCH_APIC_HWINT_xx 41
    
karch_apic_hwint_42:
    KARCH_APIC_HWINT_xx 42
    
karch_apic_hwint_43:
    KARCH_APIC_HWINT_xx 43
    
karch_apic_hwint_44:
    KARCH_APIC_HWINT_xx 44
    
karch_apic_hwint_45:
    KARCH_APIC_HWINT_xx 45
    
karch_apic_hwint_46:
    KARCH_APIC_HWINT_xx 46
    
karch_apic_hwint_47:
    KARCH_APIC_HWINT_xx 47
    
karch_apic_hwint_48:
    KARCH_APIC_HWINT_xx 48

karch_apic_hwint_49:
    KARCH_APIC_HWINT_xx 49
        
    
karch_apic_hwint_50:
    KARCH_APIC_HWINT_xx 50
    
karch_apic_hwint_51:
    KARCH_APIC_HWINT_xx 51
    
karch_apic_hwint_52:
    KARCH_APIC_HWINT_xx 52
    
karch_apic_hwint_53:
    KARCH_APIC_HWINT_xx 53
    
karch_apic_hwint_54:
    KARCH_APIC_HWINT_xx 54
    
karch_apic_hwint_55:
    KARCH_APIC_HWINT_xx 55
    
karch_apic_hwint_56:
    KARCH_APIC_HWINT_xx 56
    
karch_apic_hwint_57:
    KARCH_APIC_HWINT_xx 57
    
karch_apic_hwint_58:
    KARCH_APIC_HWINT_xx 58

karch_apic_hwint_59:
    KARCH_APIC_HWINT_xx 59
    
    
karch_apic_hwint_60:
    KARCH_APIC_HWINT_xx 60
    
karch_apic_hwint_61:
    KARCH_APIC_HWINT_xx 61
    
karch_apic_hwint_62:
    KARCH_APIC_HWINT_xx 62
    
karch_apic_hwint_63:
    KARCH_APIC_HWINT_xx 63
    
    
karch_apic_zbint_f0:
    KARCH_APIC_INTR 0xf0
    
karch_apic_zbint_f1:
    KARCH_APIC_INTR 0xf1
    
karch_apic_zbint_f2:
    KARCH_APIC_INTR 0xf2

karch_apic_zbint_ff:
    KARCH_APIC_INTR 0xff
    
karch_apic_zbint_fe:
    KARCH_APIC_INTR 0xfe
