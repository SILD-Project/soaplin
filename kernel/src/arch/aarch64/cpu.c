/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  cpu.c - AArch64 CPU control implementation.
 */

#if defined (__aarch64__)

#include <arch/cpu.h>

void arch_init_stage1() {
}

void cpu_load_pm(pagemap_t pm) {
    
}

void cpu_invalidate_page(uint64_t vaddr) {
    
}

void hcf() {
    for (;;) {
        asm ("wfi");
    }
}

#endif