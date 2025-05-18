/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  pmm.c - Physical memory allocator
 */

#pragma once

#include <stdint.h>

#define DIV_ROUND_UP(x, y)                                                     \
  (((uint64_t)(x) + ((uint64_t)(y) - 1)) / (uint64_t)(y))
#define ALIGN_UP(x, y) (DIV_ROUND_UP(x, y) * (uint64_t)(y))
#define ALIGN_DOWN(x, y) (((uint64_t)(x) / (uint64_t)(y)) * (uint64_t)(y))

#define PMM_PAGE_SIZE 0x1000 // We are using 4kb pages.

typedef struct __pmm_page {
    struct __pmm_page *next;
} pmm_page_t;

void pmm_init();