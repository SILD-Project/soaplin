#pragma once

#include <stdint.h>

#define ACPI_RSDP_SIGNATURE "RSD PTR "
#define ACPI_RSDP_SIGNATURE_LEN 7

typedef struct {
  char sign[8];
  uint8_t checksum;
  char oem_id[6];
  uint8_t revision;
  uint32_t rsdt_addr;
} __attribute__((packed)) acpi_rsdp;

typedef struct {
  char sign[8];
  uint8_t checksum;
  char oem_id[6];
  uint8_t revision;
  uint32_t resv;

  uint32_t length;
  uint64_t xsdt_addr;
  uint8_t extended_checksum;
  uint8_t resv1[3];
} __attribute__((packed)) acpi_xsdp;

typedef struct {
  char sign[4];
  uint32_t len;
  uint8_t revision;
  uint8_t checksum;
  char oem_id[6];
  char oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((packed)) acpi_sdt;

typedef struct {
  acpi_sdt sdt;
  char table[];
} acpi_rsdt;

typedef struct {
  acpi_sdt sdt;
  char table[];
} acpi_xsdt;

void *acpi_find_table(const char *name);
void acpi_init();