/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  gdt.h - x86_64 Global Descriptor Table definitions.
 */

#pragma once

#include <stdint.h>

typedef struct tss_entry {
  uint16_t length;
  uint16_t base;
  uint8_t base1;
  uint8_t flags;
  uint8_t flags1;
  uint8_t base2;
  uint32_t base3;
  uint32_t resv;
} __attribute__((packed)) tss_entry;

typedef struct {
  uint64_t entries[5];
  struct tss_entry tss_entry;
} __attribute__((packed)) gdt_table;

typedef struct {
  uint16_t size;
  uint64_t address;
} __attribute__((packed)) gdtr;

typedef struct {
  uint32_t resv;
  uint64_t rsp[4];
  uint64_t resv1;
  uint64_t ist[7];
  uint64_t resv2;
  uint16_t resv3;
  uint16_t iopb;
} __attribute__((packed)) tssr; // 1 TSSR per CPU.

void gdt_init();