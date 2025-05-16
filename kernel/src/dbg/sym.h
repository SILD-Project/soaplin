#pragma once

#include "mm/vmm.h"

typedef struct func {
  uint64_t base;
  uint64_t ip;
  char *name;
} func;

void ksym_init();
func *ksym_fromip(uint64_t ip);