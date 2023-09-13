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
    mov rax, 2
    push rax
    mov rdi, rax
    call sqrt
    push rax
    pop rax
    mov [var_h], rax
    ; Printing number
    ; Pushing Variable
    mov rax, [var_h]
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
    ; Function declaration for printf
    global printf
printf:
    push rbp
    mov rbp, rsp
    push rdi
    ; Pushing Variable
    mov rax, [rbp - 8]
    push rax
    pop rdi
    mov rsi, [rbp - 8]
    ; Start compute length of string
    xor rcx, rcx
.length_loop:
    mov al, BYTE [rsi + rcx]
    test al, al
    je .length_done
    inc rcx
    jmp .length_loop
.length_done:
    ; End compute length of string
    mov rdx, rcx
    mov rax, 1
    mov rdi, 1
    syscall
    ; Printing newline
    mov rax, 1
    mov rdi, 1
    mov rsi, newline_str
    mov rdx, 1
    syscall
    mov rsp, rbp
    pop rbp
    ret
    ; Function declaration for printi
    global printi
printi:
    push rbp
    mov rbp, rsp
    push rdi
    push rsi
    ; Pushing Variable
    mov rax, [rbp - 8]
    push rax
    pop rdi
    mov rsi, [rbp - 8]
    ; Start compute length of string
    xor rcx, rcx
.length_loop:
    mov al, BYTE [rsi + rcx]
    test al, al
    je .length_done
    inc rcx
    jmp .length_loop
.length_done:
    ; End compute length of string
    mov rdx, rcx
    mov rax, 1
    mov rdi, 1
    syscall
    ; Printing number
    ; Pushing Variable
    mov rax, [rbp - 16]
    push rax
    pop rdi
    call dump
    mov rsp, rbp
    pop rbp
    ret
    ; Function declaration with return for pow
    global pow
pow:
    push rbp
    mov rbp, rsp
    push rdi
    push rsi
    ; Declearing variable
    ; Pushing Integer Literal
    mov rax, 1
    push rax
    pop rax
    mov [var_resultpow], rax
    ; Declearing variable
    ; Pushing Integer Literal
    mov rax, 0
    push rax
    pop rax
    mov [var_i], rax
    ; Pushing Variable
    mov rax, [var_i]
    push rax
FOR_START_0:
    ; Pushing Variable
    mov rax, [var_i]
    push rax
    ; Pushing Variable
    mov rax, [rbp - 16]
    push rax
    ; Checkign if lesser
    pop rdi
    pop rax
    cmp rax, rdi
    setl al
    push rax
    pop rax
    test al, al
    je FOR_END_0
    ; Pushing Variable
    mov rax, [var_resultpow]
    push rax
    ; Pushing Variable
    mov rax, [rbp - 8]
    push rax
    ; Multiplying
    pop rdi
    pop rax
    imul rax, rdi
    push rax
    ; Assigning variable
    pop rax
    mov [var_resultpow], rax
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
    ; Pushing Variable
    mov rax, [var_resultpow]
    push rax
    leave
    ret
    ; Function declaration with return for sqrt
    global sqrt
sqrt:
    push rbp
    mov rbp, rsp
    push rdi
    ; Pushing Variable
    mov rax, [rbp - 8]
    push rax
    ; Pushing Integer Literal
    mov rax, 0
    push rax
    ; Checkign if lesser
    pop rdi
    pop rax
    cmp rax, rdi
    setl al
    push rax
    pop rax
    cmp rax, 1
    je IF_TRUE_0
    jmp IF_FALSE_0
IF_TRUE_0:
    ; Pushing String Literal
    lea rax, [str_0_22]
    push rax
    mov rdi, rax
    call printf
    jmp IF_END_0
IF_FALSE_0:
IF_END_0:
    ; Declearing variable
    ; Pushing Variable
    mov rax, [rbp - 8]
    push rax
    pop rax
    mov [var_guess], rax
    ; Declearing variable
    ; Pushing Integer Literal
    mov rax, 0
    push rax
    pop rax
    mov [var_j], rax
    ; Pushing Variable
    mov rax, [var_j]
    push rax
FOR_START_1:
    ; Pushing Variable
    mov rax, [var_j]
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
    test al, al
    je FOR_END_1
    ; Pushing Variable
    mov rax, [var_guess]
    push rax
    ; Pushing Integer Literal
    mov rax, 2
    push rax
    ; Dividing
    pop rdi
    pop rax
    cqo
    idiv rdi
    push rax
    ; Pushing Variable
    mov rax, [rbp - 8]
    push rax
    ; Pushing Variable
    mov rax, [var_guess]
    push rax
    ; Dividing
    pop rdi
    pop rax
    cqo
    idiv rdi
    push rax
    ; Pushing Integer Literal
    mov rax, 2
    push rax
    ; Dividing
    pop rdi
    pop rax
    cqo
    idiv rdi
    push rax
    ; Adding
    pop rdi
    pop rax
    add rax, rdi
    push rax
    ; Assigning variable
    pop rax
    mov [var_guess], rax
    ; Pushing Variable
    mov rax, [var_j]
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
    mov [var_j], rax
    jmp FOR_START_1
FOR_END_1:
    ; Pushing Variable
    mov rax, [var_guess]
    push rax
    leave
    ret
segment .data
str_0_22: db "Error: negative number", 0
var_h: dq 0
var_resultpow: dq 0
var_i: dq 0
var_guess: dq 0
var_j: dq 0

newline_str: db 10
