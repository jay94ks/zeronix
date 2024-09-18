SEG_KERN_CS equ (1 * 8)

section .text

global read_cr0;
global read_cr3;
global read_cr4;
global write_cr0;
global write_cr3;
global write_cr4;
global read_cpuid;

global read_msr;
global write_msr;

global load_gdt;
global load_idt;
global load_ldt;
global load_tr;
global store_gdt;
global store_idt;
    
global load_kern_cs
global load_ds
global load_es
global load_fs
global load_gs
global load_ss

global switch_stack;

global cpu_hlt;
global cpu_cli;
global cpu_sti;
global cpu_stihlt;

global cpu_in8;
global cpu_in16;
global cpu_in32;

global cpu_out8;
global cpu_out16;
global cpu_out32;

global cpu_cmpxchg32;
global cpu_read_tsc;
global cpu_mfence;
global cpu_pause;
global cpu_nop;

read_cr0:
    push ebp
    mov ebp, esp

    mov eax, cr0
    
    mov esp, ebp
    pop ebp
    ret

read_cr3:
    push ebp
    mov ebp, esp

    mov eax, cr3
    
    mov esp, ebp
    pop ebp
    ret

read_cr4:
    push ebp
    mov ebp, esp
    
    mov eax, cr4

    mov esp, ebp
    pop ebp
    ret

write_cr0:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]

    mov cr0, eax
    jmp .done

    .done:
        mov esp, ebp
        pop ebp
        ret

write_cr3:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]

    mov cr3, eax
    jmp .done

    .done:
        mov esp, ebp
        pop ebp
        ret

write_cr4:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]

    mov cr4, eax
    jmp .done

    .done:
        mov esp, ebp
        pop ebp
        ret

read_cpuid:
    push ebp
    push ebx

    mov ebp, [esp + 12]
    mov eax, [ebp]
    
    mov ebp, [esp + 16]
    mov ebx, [ebp]
    
    mov ebp, [esp + 20]
    mov ecx, [ebp]

    mov ebp, [esp + 24]
    mov edx, [ebp]

    ; cpu id.
    db 0x0f
    db 0xa2
    
    mov ebp, [esp + 12]
    mov [ebp], eax
    
    mov ebp, [esp + 16]
    mov [ebp], ebx
    
    mov ebp, [esp + 20]
    mov [ebp], ecx
    
    mov ebp, [esp + 24]
    mov [ebp], edx

    pop ebx
    pop ebp
    ret

read_msr:
    push ebp
    mov ebp, esp

    mov ecx, [ebp + 8]
    rdmsr

    mov ecx, [ebp + 12]
    mov [ecx], edx

    mov ecx, [ebp + 16]
    mov [ecx], eax

    mov esp, ebp
    pop ebp
    ret

write_msr:
    push ebp;
    mov ebp, esp

    mov edx, [ebp + 12]
    mov eax, [ebp + 16]
    mov ecx, [ebp + 8]

    wrmsr
    mov esp, ebp
    pop ebp
    ret

load_gdt:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lgdt [eax]

    mov esp, ebp
    pop ebp
    ret

load_idt:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret

load_ldt:
    push ebp
    mov ebp, esp

    lldt [ebp + 8]

    mov esp, ebp
    pop ebp
    ret

load_tr:
    push ebp
    mov ebp, esp

    ltr [ebp + 8]

    mov esp, ebp
    pop ebp
    ret
    
store_gdt:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    sgdt [eax]

    mov esp, ebp
    pop ebp
    ret
    
store_idt:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    sidt [eax]

    mov esp, ebp
    pop ebp
    ret

load_kern_cs:
    push ebp
    mov ebp, esp
    ; mov eax, [ebp + 8] ; ??

    jmp SEG_KERN_CS:.newcs
    .newcs:
        mov esp, ebp
        pop ebp
        ret

; --> segment loader.
%macro load_seg 1 ; DEST
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    mov %1, ax

    jmp .retval
    .retval:
        mov esp, ebp
        pop ebp
        ret
%endmacro

load_ds: 
    load_seg ds
    
load_es: 
    load_seg es

load_fs:
    load_seg fs

load_gs:
    load_seg gs

load_ss:
    load_seg ss

switch_stack:
    mov eax, [esp + 8];
    mov ecx, [esp + 4];

    mov ebp, .not_reachable
    mov esp, ecx
    jmp eax
    
    .not_reachable:
        jmp .not_reachable

cpu_hlt:
    hlt
    ret

cpu_cli:
    cli
    ret

cpu_sti:
    sti
    ret

cpu_stihlt:
    sti
    hlt
    ret

cpu_in8:
    push ebp
    mov ebp, esp
    push edx
    
    mov edx, [ebp + 8]
    mov eax, 0
    
    in al, dx

    pop edx
    mov esp, ebp
    pop ebp
    ret

cpu_in16:
    push ebp
    mov ebp, esp
    push edx
    
    mov edx, [ebp + 8]
    mov eax, 0
    
    in ax, dx

    pop edx
    mov esp, ebp
    pop ebp
    ret

cpu_in32:
    push ebp
    mov ebp, esp
    push edx
    
    mov edx, [ebp + 8]
    mov eax, 0
    
    in eax, dx

    pop edx
    mov esp, ebp
    pop ebp
    ret

cpu_out8:
    push ebp
    mov ebp, esp
    push edx
    
    mov edx, [ebp + 8]
    mov al,  [ebp + 12]

    out dx, al

    pop edx
    mov esp, ebp
    pop ebp
    ret
    
cpu_out16:
    push ebp
    mov ebp, esp
    push edx
    
    mov edx, [ebp + 8]
    mov ax,  [ebp + 12]

    out dx, ax

    pop edx
    mov esp, ebp
    pop ebp
    ret
    
cpu_out32:
    push ebp
    mov ebp, esp
    push edx
    
    mov edx, [ebp + 8]
    mov eax, [ebp + 12]

    out dx, eax

    pop edx
    mov esp, ebp
    pop ebp
    ret

cpu_cmpxchg32:
    push ebp
    mov ebp, esp
    
    push edi
    push edx

    ; [ebp + 8] : ptr.
    ; [ebp + 12]: exp.
    ; [ebp + 16]: val.

    mov eax, [ebp + 12]
    mov edi, [ebp + 8]
    mov edx, [ebp + 16]

    lock cmpxchg [edi], edx
    
    ; eax = result: exp or val.

    pop edx
    pop edi

    mov esp, ebp
    pop ebp
    ret

cpu_read_tsc:
    push edx
    push eax

    db 0x0f
    db 0x31

    push ebp

    mov ebp, [esp + 16]
    mov [ebp], edx

    mov ebp, [esp + 20]
    mov [ebp], eax
    
    pop ebp
    pop eax
    pop edx
    ret

cpu_mfence:
    mfence
    ret

cpu_pause:
    pause
    ret

cpu_nop:
    nop
    ret