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