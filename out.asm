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
    ; Pushing String Literal
    lea rax, [str_0_4]
    push rax
    pop rax
    mov [var_s], rax
    ; Pushing String Literal
    lea rax, [str_1_4]
    push rax
    mov rdi, rax
    ; Pushing Integer Literal
    mov rax, 0
    push rax
    mov rsi, rax
    call printi
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
segment .data
str_0_4: db "i = ", 0
str_1_4: db "i = ", 0
var_s: dq 0

newline_str: db 10
