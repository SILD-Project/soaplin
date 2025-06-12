/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  ioapic.c - I/O APIC implementation.
 */

#include <stdint.h>
#include <stddef.h>

#include <acpi/madt.h>
#include <arch/x86_64/ioapic.h>
#include <lib/log.h>
#include <mm/pmm.h>

static void     __ioapic_write(acpi_madt_ioapic_t *ioapic, uint32_t reg, uint32_t val) {
    *(volatile uint32_t *)(higher_half(ioapic->ioapic_addr)) = reg; 
    *(volatile uint32_t *)(higher_half(ioapic->ioapic_addr+0x10)) = val; 
}
static uint32_t __ioapic_read(acpi_madt_ioapic_t *ioapic, uint32_t reg) {
    *(volatile uint32_t *)(higher_half(ioapic->ioapic_addr)) = reg; 
    return *(volatile uint32_t *)(higher_half(ioapic->ioapic_addr+0x10));
}

void ioapic_init() {
    acpi_madt_ioapic_t *ioapic = madt_ioapic_list[0];

    uint32_t value = __ioapic_read(ioapic, IOAPIC_VERSION);
    uint32_t count = (value >> 16) & 0xFF;

    for (uint8_t i = 0; i <= count; ++i) {
        __ioapic_write(ioapic, 
            IOAPIC_REDTBL+2 * i, 
            0x00010000 | (32 + i));
        __ioapic_write(ioapic,
            IOAPIC_REDTBL + 2 * i + 1, 0);
    }

    trace("ioapic: Initialized\n");
}

uint32_t ioapic_gsi_count(acpi_madt_ioapic_t* ioapic) {
  return (__ioapic_read(ioapic, 1) & 0xff0000) >> 16;
}

acpi_madt_ioapic_t *ioapic_get_gsi(uint32_t gsi) {
  for (uint32_t i = 0; i < madt_ioapic_len; i++) {
    acpi_madt_ioapic_t* ioapic = madt_ioapic_list[i];
    if (ioapic->gsi_base <= gsi && ioapic->gsi_base + ioapic_gsi_count(ioapic) > gsi)
      return ioapic;
  }
  return NULL;
}

void ioapic_redirect_gsi(uint32_t lapic_id, uint8_t vec, uint32_t gsi, uint16_t flags, bool mask) {
  acpi_madt_ioapic_t* ioapic = ioapic_get_gsi(gsi);

  uint64_t redirect = vec;

  if ((flags & (1 << 1)) != 0) {
    redirect |= (1 << 13);
  }

  if ((flags & (1 << 3)) != 0) {
    redirect |= (1 << 15);
  }

  if (mask) redirect |= (1 << 16);
  else redirect &= ~(1 << 16);

  redirect |= (uint64_t)lapic_id << 56;

  uint32_t redtbl = (gsi - ioapic->gsi_base) * 2 + 16;
  __ioapic_write(ioapic, redtbl, (uint32_t)redirect);
  __ioapic_write(ioapic, redtbl + 1, (uint32_t)(redirect >> 32));
}

void ioapic_redir_irq(uint32_t lapic_id, uint8_t vec, uint8_t irq, bool mask) {
  uint8_t idx = 0;
  acpi_madt_iso_t* iso = NULL;

  for (idx = 0; idx < madt_iso_len; ++idx) {
    iso = madt_iso_list[idx];
    if (iso->source == irq) { ioapic_redirect_gsi(lapic_id, vec, iso->gsi, iso->flags, mask); return; }
  }
  ioapic_redirect_gsi(lapic_id, vec, irq, 0, mask);
}