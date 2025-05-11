bits 64
section .text
    global _start

_start:
    mov rax, 10
.hey:
    add rax, 1
    jmp .hey