section .text_smp

GDT_MAX     equ 22
MAX_IDT     equ 256
SEG_KERN_CS equ (1 * 8)
SEG_KERN_DS equ (2 * 8)

global smp_ap_entry;
global __smp_ap_base;
global __smp_ap_id;
global __smp_ap_pt;
global __smp_ap_gdt;
global __smp_ap_idt;
global __smp_ap_gdt_sys;
global __smp_ap_idt_sys;
global __smp_ap_gdt_tab;
global __smp_ap_idt_tab;
global __smp_entry_end;

extern karch_smp_init_ap32;

align 4096

smp_ap_entry:
    bits 16
    cli

    ; -------------- 16 bits addressing mode -----------------
    mov ax, cs
    mov ds, ax      ; 0x7000
    
    ; --> read temporary table descriptors.
    nop
    mov ax, (__smp_ap_gdt - smp_ap_entry)
    nop
    mov bx, (__smp_ap_idt - smp_ap_entry)
    
    ; --> load temporary tables.
    mov di, ax
	lgdt	[di]
    
    mov di, bx
    lidt	[di]

    ; switch to protected mode.
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; cr4 PSE on, cr4 PGE off
    mov eax, cr4
    or eax, 0x00000010
    and eax, ~0x00000080
    mov cr4, eax

    ; -------------- 32 bits addressing mode -----------------
    ; load boot cr3 and turn PG, so CPU can see all memory.
    mov ax, [__smp_ap_pt - smp_ap_entry + 2]
    shl eax, 16

    mov ax, [__smp_ap_pt - smp_ap_entry]
    mov cr3, eax

    ; enable paging.
    mov eax, cr0
    or eax, 0x80000000
    ; and eax, ~0x60000000
    nop
    mov cr0, eax

    ; turn on PGE.
    mov eax, cr4
    or eax, 0x00000080
    mov cr4, eax

    ; --> read system table descriptors.
    nop
    mov ax, (__smp_ap_gdt_sys - smp_ap_entry)

    nop
    mov bx, (__smp_ap_idt_sys - smp_ap_entry)
    
    ; --> load 32-bit gdt and idt.
    mov di, ax
	lgdt	[di]
    
    mov di, bx
    lidt	[di]

    ; jump to highly mapped kernel.

    nop
    jmp dword SEG_KERN_CS:karch_smp_startup_ap32
    bits 32
    
    nop

align 4
__smp_ap_id:
    resb 4

__smp_ap_pt:
    resb 4
    
__smp_ap_gdt:
    resb 8
    
__smp_ap_idt:
    resb 8
    
__smp_ap_gdt_sys:
    resb 8
    
__smp_ap_idt_sys:
    resb 8

__smp_ap_gdt_tab:
    resb (GDT_MAX * 8)

__smp_ap_idt_tab:
    resb (MAX_IDT * 8)

__smp_entry_end:


section .text
extern _kstack_tail ; --> boot stack, defined at arch/x86/boot/src/head.asm.
extern karch_load_segs
extern karch_smp_boot_ap32

karch_smp_startup_ap32:
    mov ax, SEG_KERN_DS
    mov ds, ax
    mov ss, ax

    mov esp, _kstack_tail

    call karch_load_segs
	jmp	karch_smp_boot_ap32

    ; unreachable.
    .hang:
        hlt
        jmp .hang
