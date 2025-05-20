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
} __attribute__((packed)) acpi_rsdp_t;

typedef struct {
    acpi_rsdp_t base;
    uint32_t len;
    uint64_t xsdt_addr;
    char chksumex;
} __attribute__((packed)) acpi_xsdp_t;

typedef struct {
    char sign[4];
    uint32_t len;
    uint8_t rev;
    uint8_t chksum;
    char oemid[6];
    char oemtabid[8];
    uint32_t oemrev;
    uint32_t creaid;
    uint32_t crearev;
} __attribute__((packed)) acpi_sdt_hdr_t;

typedef struct {
    acpi_sdt_hdr_t hdr;
    char entries[];
} acpi_rsdt_t;

typedef struct {
    acpi_sdt_hdr_t hdr;
    char entries[];
} acpi_xsdt_t;

void *acpi_find_table(char *sign);
void  acpi_init();