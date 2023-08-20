global _start
_start:
    mov r10, 5
    imul r10, 3
    mov r13, 4
    imul r13, 2
    mov r14, r10
    add r14, r13
    mov rdi, r14
    mov rax, 60
    syscall
