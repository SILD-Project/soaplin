#include "sys/acpi.h"
#include <stdint.h>
#include <sys/acpi/madt.h>
#include <sys/errhnd/panic.h>
#include <sys/log.h>

madt_ioapic *acpi_madt_ioapic_list[256] = {0};
madt_iso *acpi_madt_iso_list[256] = {0};

uint32_t acpi_madt_ioapic_length = 0;
uint32_t acpi_madt_iso_length = 0;

uint64_t *acpi_lapic_addr = 0;

void madt_init() {
  void *addr = acpi_find_table("APIC");
  if (!addr)
    panic("madt: Failed to find MADT table!");

  acpi_madt *madt = (acpi_madt *)addr;

  uint64_t offset = 0;
  int i = 0;

  while (1) {
    if (offset > madt->sdt.len - sizeof(acpi_madt))
      break;

    madt_entry *entry = (madt_entry *)(madt->table + offset);

    if (entry->type == 0)
      i++;
    else if (entry->type == 1)
      acpi_madt_ioapic_list[acpi_madt_ioapic_length++] = (madt_ioapic *)entry;
    else if (entry->type == 2)
      acpi_madt_iso_list[acpi_madt_iso_length++] = (madt_iso *)entry;
    else if (entry->type == 5)
      acpi_lapic_addr = (uint64_t *)((madt_lapic_addr *)entry)->phys_lapic;

    offset += entry->length;
  }
}