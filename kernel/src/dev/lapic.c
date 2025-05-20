/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  lapic.c - Local APIC implementation.
 */

#include "dev/lapic.h"
#include "arch/x86_64/idt.h"
#include "lib/log.h"
#include "mm/pmm.h"
#include <stdint.h>

static uint64_t __lapic_pbase = 0xfee00000;
static uint64_t __lapic_vbase;

static void     __lapic_write(uint32_t reg, uint32_t val) { *(volatile uint32_t *)(__lapic_vbase + reg) = val; }
static uint32_t __lapic_read(uint32_t reg) { return *(volatile uint32_t *)(__lapic_vbase + reg); }
static void     __lapic_write_svr(uint32_t reg, lapic_svr_entry e) { *(volatile uint32_t *)(__lapic_vbase + reg) = *(uint32_t*)&e; }

void lapic_init() {
    __lapic_vbase = higher_half(__lapic_pbase);
    __lapic_write_svr(LAPIC_SVR, (lapic_svr_entry){ .swenabled = true, .vec = IDT_SPURIOUS_INT });
    trace("lapic: Initialized\n");
}

void lapic_eoi() {
    __lapic_write(LAPIC_EOI, 0x0);
}

uint32_t lapic_get_id() {
    return __lapic_read(LAPIC_ID) >> LAPIC_ICDESTSHIFT;
}