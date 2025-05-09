bits 64
section .text
    global _start

_start:
    int 0x80
    ; tell the kernel to exit the process.
    mov rax, 10
    int 0x80
    ret