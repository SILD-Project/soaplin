bits 64
section .text
    global _start

_start:
    int 0x80
    int 0x80
    int 0x80
    int 0x80
    int 0x80
    int 0x80
    int 0x80
.hey:
    jmp .hey