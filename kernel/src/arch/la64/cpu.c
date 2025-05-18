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

void cpu_load_pm(pagemap_t pm) {
    
}

void cpu_invalidate_page(pagemap_t pm, uint64_t vaddr) {
    
}

void hcf() {
    for (;;) {
        asm ("idle 0");
    }
}

#endif