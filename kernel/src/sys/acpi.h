#pragma once

#include <stdint.h>

#define ACPI_RSDP_SIGNATURE "RSD PTR "
#define ACPI_RSDP_SIGNATURE_LEN 7

typedef struct __acpi_table_header {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} acpi_table_header_t;

typedef struct __rsdp {
  char signature[8];
  uint8_t chksum;
  char oemid[6];
  uint8_t rev;
  uint32_t rsdt_addr;
} __attribute__((packed)) rsdp_t;

typedef struct __xsdp {
  char signature[8];
  uint8_t chksum;
  char oemid[6];
  uint8_t rev;
  uint32_t rsdt_addr; // deprecated since version 2.0

  uint32_t len;
  uint64_t xsdt_addr;
  uint8_t chksum_ex;
  uint8_t reserved[3];
} __attribute__((packed)) xsdp_t;

typedef struct __rsdt {
  acpi_table_header_t h;
  uint32_t *PointerToOtherSDT;
} rsdt_t;

typedef struct __xsdt {
  acpi_table_header_t h;
  uint64_t *PointerToOtherSDT;
} xsdt_t;

void acpi_init();