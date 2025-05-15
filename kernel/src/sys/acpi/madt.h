#pragma once

#include "sys/acpi.h"
#include <stdint.h>
typedef struct {
  acpi_sdt sdt;

  uint32_t lapic_address;
  uint32_t flags;

  char table[];
} acpi_madt;

typedef struct {
  uint8_t type;
  uint8_t length;
} madt_entry;

typedef struct {
  madt_entry un;
  uint8_t cpu_id;
  uint8_t apic_id;
  uint32_t flags;
} madt_cpu_lapic;

typedef struct {
  madt_entry un;
  uint8_t apic_id;
  uint8_t reserved;
  uint32_t apic_addr;
  uint32_t gsi_base;
} madt_ioapic;

typedef struct {
  madt_entry un;
  uint8_t bus_src;
  uint8_t irq_src;
  uint32_t gsi;
  uint16_t flags;
} madt_iso;

typedef struct {
  madt_entry un;
  uint16_t reserved;
  uint64_t phys_lapic;
} madt_lapic_addr;

extern madt_ioapic* acpi_madt_ioapic_list[256];
extern madt_iso* acpi_madt_iso_list[256];

extern uint32_t acpi_madt_ioapic_length;
extern uint32_t acpi_madt_iso_length;

extern uint64_t* acpi_lapic_addr;

void madt_init();