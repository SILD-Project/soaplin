/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  acpi.c - ACPI table lookup
 */

#include <stdbool.h>

#include <acpi/acpi.h>
#include <arch/cpu.h>
#include <boot/limine.h>
#include <lib/log.h>
#include <mm/memop.h>

static bool     __acpi_use_xsdt;
static uint64_t __acpi_rsdt_addr;

void acpi_init() {
    acpi_rsdp_t *rsdp = (acpi_rsdp_t*)limine_get_rsdp();
    if (memcmp(rsdp->sign, "RSD PTR ", 8))
    {
        fatal("acpi: ACPI RSDP is corrupt\n");
        hcf();
    }

    if (rsdp->rev >= 2)
    {
        trace("acpi: ACPI v2.0 or later detected: Using XSDT.\n");

        acpi_xsdp_t *xsdp = (acpi_xsdp_t *)rsdp;
        __acpi_use_xsdt = 1;
        __acpi_rsdt_addr = xsdp->xsdt_addr;

        goto initialized;
    }

    __acpi_use_xsdt = 0;
    __acpi_rsdt_addr = rsdp->rsdt_addr; // Do not use a pointer, to shut up the compiler.

initialized:
    trace("acpi: Initialized!\n");
}