segment .text
dump:
    push    rbp
    mov     rbp, rsp
    sub     rsp, 64
    mov     QWORD  [rbp-56], rdi
    mov     QWORD  [rbp-8], 1
    mov     eax, 32
    sub     rax, QWORD  [rbp-8]
    mov     BYTE  [rbp-48+rax], 10
.L2:
    mov     rcx, QWORD  [rbp-56]
    mov  rdx, -3689348814741910323
    mov     rax, rcx
    mul     rdx
    shr     rdx, 3
    mov     rax, rdx
    sal     rax, 2
    add     rax, rdx
    add     rax, rax
    sub     rcx, rax
    mov     rdx, rcx
    mov     eax, edx
    lea     edx, [rax+48]
    mov     eax, 31
    sub     rax, QWORD  [rbp-8]
    mov     BYTE  [rbp-48+rax], dl
    add     QWORD  [rbp-8], 1
    mov     rax, QWORD  [rbp-56]
    mov     rdx, -3689348814741910323
    mul     rdx
    mov     rax, rdx
    shr     rax, 3
    mov     QWORD  [rbp-56], rax
    cmp     QWORD  [rbp-56], 0
    jne     .L2
    mov     eax, 32
    sub     rax, QWORD  [rbp-8]
    lea     rdx, [rbp-48]
    lea     rcx, [rdx+rax]
    mov     rax, QWORD  [rbp-8]
    mov     rdx, rax
    mov     rsi, rcx
    mov     edi, 1
    mov     rax, 1
    syscall
    nop
    leave
    ret
global _start
_start:
    ; Declearing variable
    ; Pushing Integer Literal
    mov rax, 1
    push rax
    ; Pushing Integer Literal
    mov rax, 2
    push rax
    ; Adding
    pop rdi
    pop rax
    add rax, rdi
    push rax
    ; Declearing variable
    ; Pushing Variable
    push QWORD [rsp + 0]
    ; Declearing variable
    ; Pushing Integer Literal
    mov rax, 10
    push rax
    ; Pushing Variable
    push QWORD [rsp + 0]
FOR_START_0:
    ; Pushing Variable
    push QWORD [rsp + 8]
    ; Pushing Integer Literal
    mov rax, 0
    push rax
    ; Checkign if greater
    pop rdi
    pop rax
    cmp rax, rdi
    setg al
    push rax
    pop rax
    cmp rax, 0
    je FOR_END_0
    ; Pushing String Literal
    lea rax, [str_0_4]
    push rax
    pop rdi
    mov rsi, rdi
    mov rax, 1
    mov rdi, 1
    mov rdx, 4
    syscall
    ; Printing number
    ; Pushing Variable
    push QWORD [rsp + 8]
    pop rdi
    call dump
    ; Assigning variable
    ; Pushing Variable
    push QWORD [rsp + 8]
    ; Pushing Integer Literal
    mov rax, 1
    push rax
    ; Subtracting
    pop rdi
    pop rax
    sub rax, rdi
    push rax
    pop rax
    mov QWORD [rsp + 8], rax
    jmp FOR_START_0
FOR_END_0:
    ; Pushing Integer Literal
    mov rax, 0
    push rax
    mov rax, 60
    pop rdi
    syscall
    mov rax, 60
    mov rdi, 0
    syscall
segment .data
str_0_4: db "i = ", 0
