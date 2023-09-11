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
    mov rax, 20
    push rax
    pop rax
    mov [var_o], rax
    ; Declearing variable
    ; Pushing Integer Literal
    mov rax, 0
    push rax
    pop rax
    mov [var_i], rax
    ; Declearing variable
    ; Pushing Integer Literal
    mov rax, 2
    push rax
    ; Pushing Integer Literal
    mov rax, 1
    push rax
    ; Adding
    pop rdi
    pop rax
    add rax, rdi
    push rax
    pop rax
    mov [var_x], rax
    ; Pushing Variable
    mov rax, [var_i]
    push rax
FOR_START_0:
    ; Pushing Variable
    mov rax, [var_i]
    push rax
    ; Pushing Integer Literal
    mov rax, 10
    push rax
    ; Checkign if lesser
    pop rdi
    pop rax
    cmp rax, rdi
    setl al
    push rax
    pop rax
    cmp rax, 0
    je FOR_END_0
    ; Printing number
    ; Pushing Variable
    mov rax, [var_i]
    push rax
    pop rdi
    call dump
    ; Pushing Variable
    mov rax, [var_i]
    push rax
    ; Pushing Integer Literal
    mov rax, 1
    push rax
    ; Adding
    pop rdi
    pop rax
    add rax, rdi
    push rax
    ; Assigning variable
    pop rax
    mov [var_i], rax
    jmp FOR_START_0
FOR_END_0:
    ; Printing number
    ; Pushing Variable
    mov rax, [var_o]
    push rax
    pop rdi
    call dump
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
var_o: dq 0
var_i: dq 0
var_x: dq 0

