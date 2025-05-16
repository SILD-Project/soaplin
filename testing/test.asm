bits 64

section .text
    global _start

_start:
    mov rax, 1      ; write syscall number
    mov rdi, 1      ; stdout file descriptor
    mov rsi, msg    ; pointer to message
    mov rdx, msg_len ; message length
    syscall

.loop:
    jmp .loop

section .data
    msg db "Hello, World. Technically, this should work on both Soaplin & Linux.", 10    ; 10 is newline
    msg_len equ $ - msg