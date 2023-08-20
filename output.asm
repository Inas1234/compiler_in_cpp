global _start
_start:
    mov rax, 60
    add rax, 2
    mov rdi, 42
    syscall
