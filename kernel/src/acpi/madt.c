/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  madt.c - MADT lookup
 */

#include "acpi/acpi.h"
#include "lib/log.h"
#include <acpi/madt.h>

acpi_madt_ioapic_t* madt_ioapic_list[128];
acpi_madt_iso_t*    madt_iso_list[128];

uint32_t madt_ioapic_len = 0;
uint32_t madt_iso_len    = 0;

uint64_t *madt_lapic_addr = (uint64_t *)0;

void madt_init() {
    void *addr = acpi_find_table("APIC");
    acpi_madt_t *madt = (acpi_madt_t *)addr;
    uint64_t offset = 0;

    while (1) {
        if (offset > madt->hdr.len - sizeof(acpi_madt_t))
            break;

        madt_entry *entry = (madt_entry *)(madt->table + offset);

        if (entry->type == 1)
            madt_ioapic_list[madt_ioapic_len++] = (acpi_madt_ioapic_t *)entry;
        else if (entry->type == 2)
            madt_iso_list[madt_iso_len++] = (acpi_madt_iso_t *)entry;
        else if (entry->type == 5)
            madt_lapic_addr = (uint64_t *)((acpi_madt_lapic_addr_override_t *)entry)->plapic;

        offset += entry->len;
    }

    trace("madt: Initialized\n");
}
