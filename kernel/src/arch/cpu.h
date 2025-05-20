/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  cpu.h - CPU control and management functions.
 */

#pragma once

#include "mm/paging.h"

// Stage 1 initialization: Core components (such as the GDT & IDT on x86_64...)
void arch_init_stage1();

// Load a pagemap
void cpu_load_pm(pagemap_t pm);

// Invalidate a page table entry
void cpu_invalidate_page(uint64_t vaddr);

// Initialize SMP. The implementations are in arch since the Limine
// structure changes depending on the architecture.
void cpu_init_smp();

// Initialize the CPU's timer
void cpu_init_timer();

// Disable interrupts and halt the system.
void hcf();