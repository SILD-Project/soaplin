/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  lapic.h - Local APIC definitions.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define LAPIC_ID  0x20
#define LAPIC_SVR 0xF0
#define LAPIC_EOI 0xB0

#define LAPIC_ICDESTSHIFT 24

typedef struct {
    uint8_t vec;
    bool swenabled;
} lapic_svr_entry;

void lapic_init();
void lapic_eoi();
uint32_t lapic_get_id();