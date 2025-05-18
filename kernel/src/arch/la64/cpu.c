/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  cpu.c - LoongArch64 CPU control implementation.
 */

#if defined (__loongarch64)

#include <arch/cpu.h>

void arch_init_stage1() {
}

void hcf() {
    for (;;) {
        asm ("idle 0");
    }
}

#endif