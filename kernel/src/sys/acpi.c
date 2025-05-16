#include "limine.h"
#include "mm/pmm.h"
#include "sys/log.h"
#include <mm/memop.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/acpi.h>
#include <sys/errhnd/panic.h>

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_rsdp_request
    rsdp_req = {.revision = 0, .id = LIMINE_RSDP_REQUEST};

static int __acpi_uses_xsdt = 0;
static void *__acpi_rsdt_ptr;

void *acpi_find_table(const char *name) {
  if (!__acpi_uses_xsdt) {
    acpi_rsdt *rsdt = (acpi_rsdt *)__acpi_rsdt_ptr;
    uint32_t entries = (rsdt->sdt.len - sizeof(rsdt->sdt)) / 4;

    for (uint32_t i = 0; i < entries; i++) {
      acpi_sdt *sdt = (acpi_sdt *)HIGHER_HALF(*((uint32_t *)rsdt->table + i));
      if (!memcmp(sdt->sign, name, 4))
        return (void *)sdt;
    }
    return NULL;
  }

  acpi_xsdt *xsdt = (acpi_xsdt *)__acpi_rsdt_ptr;
  uint32_t entries = (xsdt->sdt.len - sizeof(xsdt->sdt)) / 8;

  for (uint32_t i = 0; i < entries; i++) {
    acpi_sdt *sdt = (acpi_sdt *)HIGHER_HALF(*((uint64_t *)xsdt->table + i));
    if (!memcmp(sdt->sign, name, 4)) {
      return (void *)sdt;
    }
  }

  return NULL;
}

void acpi_init() {
  acpi_rsdp *rsdp = (acpi_rsdp *)HIGHER_HALF(rsdp_req.response->address);

  if (memcmp(rsdp->sign, "RSD PTR", 7))
    panic("acpi: Invalid RSDP signature!");

  if (rsdp->revision != 0) {
    __acpi_uses_xsdt = 1;
    acpi_xsdp *xsdp = (acpi_xsdp *)rsdp;
    __acpi_rsdt_ptr = (void *)HIGHER_HALF(xsdp->xsdt_addr);
    return;
  }

  __acpi_rsdt_ptr = (void *)HIGHER_HALF(rsdp->rsdt_addr);
}