/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  cpu.h - CPU control and management functions.
 */

#pragma once

// Stage 1 initialization: Core components (such as the GDT & IDT on x86_64...)
void arch_init_stage1();

// Disable interrupts and halt the system.
void hcf();