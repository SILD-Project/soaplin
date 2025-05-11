#include "limine.h"
#include "mm/pmm.h"
#include "sys/log.h"
#include <mm/memop.h>
#include <stddef.h>
#include <sys/acpi.h>

__attribute__((
    used,
    section(".limine_requests"))) static volatile struct limine_rsdp_request
    rsdp_req = {.revision = 0, .id = LIMINE_RSDP_REQUEST};

rsdp_t *rsdp;
int acpi_available = 0;

int is_xsdt = 0;
xsdt_t *xsdt;
rsdt_t *rsdt;
int item_count = 0;

static int acpi_validate_rsdp(char *byte_array, size_t size) {
  uint32_t sum = 0;
  for (int i = 0; i < size; i++) {
    sum += byte_array[i];
  }
  return (sum & 0xFF) == 0;
}

void *acpi_find_table(char *sign, int sign_size) {
  if (!acpi_available)
    return NULL;

  if (is_xsdt) {
    for (int i = 0; i < item_count; i++) {
      uint64_t *lst =
          (uint64_t *)HIGHER_HALF((uint64_t)xsdt->PointerToOtherSDT);
      acpi_table_header_t *ptr = (acpi_table_header_t *)HIGHER_HALF(lst[i]);

      if (!memcmp(ptr->Signature, sign, sign_size))
        return (void *)ptr;
    }
  } else {
    for (int i = 0; i < item_count; i++) {
      acpi_table_header_t *ptr = (acpi_table_header_t *)HIGHER_HALF(
          (uint64_t)rsdt->PointerToOtherSDT[i]);
      if (!memcmp(ptr->Signature, sign, sign_size))
        return (void *)ptr;
    }
  }

  return NULL;
}

void acpi_init() {
  rsdp_t *rsdp = (rsdp_t *)HIGHER_HALF(rsdp_req.response->address);

  if (!rsdp) {
    log("acpi - not available: RSDP is NULL!\n");
    return;
  }

  if (rsdp->rev < 2) {
    if (!acpi_validate_rsdp((char *)rsdp, sizeof(rsdp_t))) {
      log("acpi - not available: Was the RSDP hijacked?\n");
      return;
    }
    rsdt = (rsdt_t *)HIGHER_HALF((uint64_t)rsdp->rsdt_addr);
    log("acpi - RSDT found at %p\n", rsdt);
    item_count = (rsdt->h.Length - sizeof(acpi_table_header_t)) / 4;
    log("acpi - RSDT contains %d entries\n", item_count);
  } else {
    is_xsdt = 1;
    if (!acpi_validate_rsdp((char *)rsdp, sizeof(xsdp_t))) {
      log("acpi - not available: Was the XSDP hijacked?\n");
      return;
    }

    xsdt = (xsdt_t *)HIGHER_HALF((uint64_t)((xsdp_t *)rsdp)->xsdt_addr);
    log("acpi - XSDT found at %p\n", xsdt);
    item_count = (xsdt->h.Length - sizeof(acpi_table_header_t)) / 8;
    log("acpi - XSDT contains %d entries\n", item_count);
  }

  acpi_available = 1;

  void *fadt = acpi_find_table("FACP", 4);
  if (!fadt) {
    log("acpi - FADT not found\n");
    acpi_available = 0;
    return;
  } else {
    log("acpi - FADT found at %p\n", fadt);
    log("acpi - ACPI initialized successfully\n");
  }
}