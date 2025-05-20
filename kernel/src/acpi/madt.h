/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  madt.h - MADT lookup
 */
#pragma once

#include <acpi/acpi.h>
#include <stdint.h>

typedef struct {
    acpi_sdt_hdr_t hdr;
    uint32_t lapic_addr;
    uint32_t flags;
    char table[];
} acpi_madt_t;

typedef struct {
    uint8_t type;
    uint8_t len;
} madt_entry;

typedef struct {
    madt_entry entry;
    uint8_t cpu_id;
    uint8_t lapic_id;
    uint32_t flags;
} acpi_madt_lapic_t;

typedef struct {
    madt_entry entry;
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t ioapic_addr;
    uint32_t gsi_base;
} acpi_madt_ioapic_t;

typedef struct {
    madt_entry entry;
    uint8_t bus;
    uint8_t source;
    uint32_t gsi;
    uint16_t flags;
} acpi_madt_iso_t; // iso = interrupt source override

typedef struct {
    madt_entry entry;
    uint16_t reserved;
    uint64_t plapic;
} acpi_madt_lapic_addr_override_t;

extern acpi_madt_ioapic_t* madt_ioapic_list[128];
extern acpi_madt_iso_t* madt_iso_list[128];

extern uint32_t madt_ioapic_len;
extern uint32_t madt_iso_len;

extern uint64_t *lapic_addr;

void madt_init();