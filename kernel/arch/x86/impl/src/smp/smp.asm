section .text ; --> this code'll be placed at 0x004xxxxx.

GDT_MAX     equ 22
SEG_KERN_CS equ (1 * 8)

global smp_ap_entry;
global __smp_ap_id;
global __smp_ap_pt;
global __smp_ap_gdt;
global __smp_ap_idt;
global __smp_ap_gdt_tab;
global __smp_ap_idt_tab;
global __smp_entry_end;

extern karch_smp_init_ap32;

align 4096
smp_ap_entry:
    cli

    mov ax, cs
    mov ds, ax

    ; --> get absolute address (set by `karch_smp_start_ap`).
    mov eax, __smp_ap_lower
    mov ebx, [eax]

    ; --> load gdt and idt prepared by bsp
    mov eax, __smp_ap_gdt
    sub eax, smp_ap_entry   ; --> convert as relative address.
    add eax, ebx            ; --> convert to absolute address.
    push eax     
    
	lgdt	[esp + 4]
    pop eax

    ; --> get absolute address (set by `karch_smp_start_ap`).
    mov eax, __smp_ap_lower
    mov ebx, [eax]

    ; --> load idt and idt prepared by bsp
    mov eax, __smp_ap_idt
    sub eax, smp_ap_entry   ; --> convert as relative address.
    add eax, ebx            ; --> convert to absolute address.
    push eax     
    
	lidt	[esp + 4]
    pop eax

    ; switch to protected mode.
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; cr4 PSE on, cr4 PGE off
    mov eax, cr4
    or eax, 0x00000010
    and eax, 0x00000080
    mov cr4, eax

    ; load boot cr3 and turn PG, so CPU can see all memory.
    mov eax, __smp_ap_pt
    sub eax, smp_ap_entry
    mov cr3, eax

    ; enable paging.
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

    ; turn on PGE.
    mov eax, cr4
    or eax, 0x00000080
    mov cr4, eax

    ; jump to highly mapped kernel.
    jmp SEG_KERN_CS:karch_smp_init_ap32

align 4
__smp_ap_id:
    resb 4

__smp_ap_pt:
    resb 4
    
__smp_ap_gdt:
    resb 8
    
__smp_ap_idt:
    resb 8

__smp_ap_gdt_tab:
    resb (GDT_MAX * 8)

__smp_ap_idt_tab:
    resb (256 * 8)

__smp_entry_end:
