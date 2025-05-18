/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  pmm.c - Physical memory allocator
 */

#pragma once

#include <stdint.h>
#include <boot/limine.h>

#define DIV_ROUND_UP(x, y)                                                     \
  (((uint64_t)(x) + ((uint64_t)(y) - 1)) / (uint64_t)(y))
#define ALIGN_UP(x, y) (DIV_ROUND_UP(x, y) * (uint64_t)(y))
#define ALIGN_DOWN(x, y) (((uint64_t)(x) / (uint64_t)(y)) * (uint64_t)(y))

#define PMM_PAGE_SIZE 0x1000 // We are using 4kb pages.

typedef struct __pmm_page {
    struct __pmm_page *next;
} pmm_page_t;

typedef struct __pmm_region {
    uint64_t base;
    uint64_t length;
    struct __pmm_region *next;
} pmm_region_t;

inline uint64_t higher_half(uint64_t addr) {
    uint64_t hhdm_off = limine_get_hhdm_offset();
    if (addr > hhdm_off)
        return addr;

    return addr + hhdm_off;
}

inline uint64_t physical(uint64_t addr) {
    uint64_t hhdm_off = limine_get_hhdm_offset();
    if (addr < hhdm_off)
        return addr;

    return addr - hhdm_off;
}

void pmm_free_page(void *mem);
void *pmm_alloc_page();
void pmm_init();