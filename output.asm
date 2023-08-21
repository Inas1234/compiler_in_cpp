section .data
section .text
global _start
_start:
    mov r10, 5
    imul r10, 2
    mov r11, 2
    add r11, r10
    mov r12, 6
    add r12, r11
    mov r13, 5
    add r13, r12
    mov rdi, r13
    mov rax, 60
    syscall
