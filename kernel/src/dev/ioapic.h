/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  ioapic.c - I/O APIC definitions.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define IOAPIC_VERSION 0x1
#define IOAPIC_REDTBL 0x10

void ioapic_init();
void ioapic_redir_irq(uint32_t lapic_id, uint8_t vec, uint8_t irq, bool mask);