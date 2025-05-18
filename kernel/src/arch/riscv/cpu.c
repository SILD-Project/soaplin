/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  cpu.c - RISC-V CPU control implementation.
 */

#if defined (__riscv)

#include <arch/cpu.h>

void arch_init_stage1() {
}

void hcf() {
    for (;;) {
        asm ("wfi");
    }
}

#endif