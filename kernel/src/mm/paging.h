/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  pg.c - Virtual memory manager
 */

#pragma once

// Page flags
#include <stdint.h>

#define PTE_ADDR_MASK 0x000ffffffffff000
#define PTE_GET_ADDR(VALUE) ((VALUE) & PTE_ADDR_MASK)
#define PTE_GET_FLAGS(VALUE) ((VALUE) & ~PTE_ADDR_MASK)

#define PTE_PRESENT  (1 << 0)
#define PTE_WRITE    (1 << 1)
#define PTE_USER     (1 << 2)
#define PTE_NX       (1ULL << 63) // NX = No eXecute.

typedef uint64_t *pagemap_t;

void pg_init();
pagemap_t pg_alloc_pm();
void pg_free_pm(pagemap_t pm);

void pg_load_pm(pagemap_t pm);
void pg_map(pagemap_t pm, uint64_t vaddr, uint64_t paddr, uint64_t flags);
void pg_map_user(pagemap_t pm, uint64_t vaddr, uint64_t paddr,
                  uint64_t flags);
void pg_unmap(pagemap_t pm, uint64_t vaddr);
void pg_protect(pagemap_t pm, uint64_t vaddr, uint64_t flags);