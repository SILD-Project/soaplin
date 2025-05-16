bits 64
section .text
    global kmain
    extern __kmain
    extern kstack

kmain:
    mov rsp, kstack+0x2000  ; supposons 16 KiB de stack
    xor rbp, rbp       ; Set %ebp to NULL
    call __kmain