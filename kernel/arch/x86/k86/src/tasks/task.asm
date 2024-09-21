section .text
global task_far_jump
global task_switch_swint

task_far_jump:
    jmp far [esp + 4]

task_switch_swint:
    int 0xf1

    ; not reachable.
    ret