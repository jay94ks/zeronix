section .text

global tpg_read_cr0;
global tpg_read_cr3;
global tpg_read_cr4;
global tpg_write_cr0;
global tpg_write_cr3;
global tpg_write_cr4;
global tpg_cpuid;

tpg_read_cr0:
    push ebp
    mov ebp, esp
    mov eax, cr0
    pop ebp
    ret

tpg_read_cr3:
    push ebp
    mov ebp, esp
    mov eax, cr3
    pop ebp
    ret

tpg_read_cr4:
    push ebp
    mov ebp, esp
    mov eax, cr4
    pop ebp
    ret

tpg_write_cr0:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]

    mov cr0, eax
    jmp .done

    .done:
        pop ebp
        ret

tpg_write_cr3:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]

    mov cr3, eax
    jmp .done

    .done:
        pop ebp
        ret

tpg_write_cr4:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]

    mov cr4, eax
    jmp .done

    .done:
        pop ebp
        ret

tpg_cpuid:
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