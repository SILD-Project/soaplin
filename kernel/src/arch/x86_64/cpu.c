/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  cpu.c - x86_64 CPU control implementation.
 */

#if defined (__x86_64__)

#include <arch/x86_64/gdt.h>
#include <arch/x86_64/idt.h>
#include <arch/cpu.h>

void arch_init_stage1() {
    gdt_init();
    idt_init();
}

void hcf() {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

#endif