bits 64
section .text
    global _start

_start:
    mov rax, 1025
    syscall
.loop:
    jmp .loop