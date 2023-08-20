section .data
section .text
global _start
_start:
    mov r10, 5
    add r10, 6
    mov rdi, r10
    mov rax, 60
    syscall
