/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  vmm.c - Virtual memory manager
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

// These are defined in the linker file.
extern char reqs_start_ld;
extern char reqs_end_ld;

extern char text_start_ld;
extern char text_end_ld;

extern char rodata_start_ld;
extern char rodata_end_ld;

extern char data_start_ld;
extern char data_end_ld;

void vmm_init();
pagemap_t vmm_alloc_pm();
void vmm_free_pm(pagemap_t pm);

void vmm_map(pagemap_t pm, uint64_t vaddr, uint64_t paddr, uint64_t flags);
void vmm_map_user(pagemap_t pm, uint64_t vaddr, uint64_t paddr,
                  uint64_t flags);
void vmm_unmap(pagemap_t pm, uint64_t vaddr);
void vmm_protect(pagemap_t pm, uint64_t vaddr, uint64_t flags);