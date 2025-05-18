/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  cpu.h - CPU control and management functions.
 */

#pragma once

#include "mm/vmm.h"

// Stage 1 initialization: Core components (such as the GDT & IDT on x86_64...)
void arch_init_stage1();

// Load a pagemap
void cpu_load_pm(pagemap_t pm);

// Invalidate a page table entry
void cpu_invalidate_page(pagemap_t pm, uint64_t vaddr);

// Disable interrupts and halt the system.
void hcf();