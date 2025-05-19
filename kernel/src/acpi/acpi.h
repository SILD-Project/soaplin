/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  acpi.h - ACPI definitions
 */

#pragma once

#include <stdint.h>

typedef struct {
    char sign[8];
    char chksum;
    char oemid[6];
    char rev;
    uint32_t rsdt_addr;
} acpi_rsdp_t;

typedef struct {
    acpi_rsdp_t base;
    uint32_t len;
    uint64_t xsdt_addr;
    char chksumex;
} acpi_xsdp_t;

void acpi_init();
void acpi_init();