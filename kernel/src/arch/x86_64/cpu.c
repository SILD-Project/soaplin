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

static uint64_t read_cr3(void)
{
    unsigned long val;
    asm volatile ( "mov %%cr3, %0" : "=r"(val) );
    return val;
}

void cpu_invalidate_page(pagemap_t pm, uint64_t vaddr) {
    uint64_t cr3 = read_cr3();
    if (physical((uint64_t)pm) != cr3)
    {
        // load the provided PM in cr3, invalidate the page and return into the previous cr3.
        cpu_load_pm(pm);
        asm volatile ( "invlpg (%0)" : : "b"(vaddr) : "memory" );
        cpu_load_pm((pagemap_t)cr3);
        return;
    }
    asm volatile ( "invlpg (%0)" : : "b"(vaddr) : "memory" );
}

void hcf() {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

#endif