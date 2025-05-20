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
#include <mm/pmm.h>
#include <stdint.h>

static bool     __acpi_use_xsdt;
static uint64_t __acpi_rsdt_addr;

void *acpi_find_table(char *sign) {
    if (__acpi_use_xsdt) {
        acpi_xsdt_t *xsdt = (acpi_xsdt_t*)__acpi_rsdt_addr;
        uint32_t entries = (xsdt->hdr.len - sizeof(xsdt->hdr)) / 8;

        for (uint32_t i = 0; i < entries; i++)
        {
            acpi_sdt_hdr_t *h = (acpi_sdt_hdr_t *)higher_half(*((uint64_t*)xsdt->entries + i));
            if (!memcmp(h->sign, sign, 4))
                return (void *) h;
        }

        return NULL;
    }

    acpi_rsdt_t *rsdt = (acpi_rsdt_t*)__acpi_rsdt_addr;
    int entries = (rsdt->hdr.len - sizeof(rsdt->hdr)) / 4;
    uint32_t *entries_hhalf = (uint32_t *)higher_half((uint64_t)rsdt->entries);

    for (int i = 0; i < entries; i++)
    {
        acpi_sdt_hdr_t *h = (acpi_sdt_hdr_t *)(entries_hhalf[i] + (i * sizeof(uint32_t)));
        if (!memcmp(h->sign, sign, 4))
            return (void *) h;
    }
    
    return NULL;
}

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
        __acpi_rsdt_addr = higher_half(xsdp->xsdt_addr);

        goto initialized;
    }

    __acpi_use_xsdt = 0;
    __acpi_rsdt_addr = higher_half(rsdp->rsdt_addr); // Do not use a pointer, to shut up the compiler.

initialized:
    trace("acpi: Initialized!\n");
}