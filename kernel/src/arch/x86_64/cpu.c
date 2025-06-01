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
#include <mm/pmm.h>
#include "mm/paging.h"

void arch_init_stage1() {
    gdt_init();
    idt_init();
}

void cpu_load_pm(pagemap_t pm) {
  if (!pm)
    return;

  __asm__ volatile("mov %0, %%cr3" : : "r"(physical((uint64_t)pm)) : "memory");
}

void cpu_invalidate_page(uint64_t vaddr) {
    asm volatile ( "invlpg (%0)" : : "b"(vaddr) : "memory" );
}

void cpu_enable_ints(int enabled) {
  if (enabled) asm("sti"); else asm("cli");
}

void hcf() {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

#endif