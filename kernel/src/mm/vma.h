/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  vma.h - Virtual memory allocator
 */

#pragma once

#include "mm/paging.h"
#include <stddef.h>

typedef struct __vma_region {
    uint64_t start;
    uint64_t pflags;

    // Length is in pages, not in bytes.
    uint64_t length;

    struct __vma_region *next;
} vma_region_t;

typedef struct {
    pagemap_t pm;
    vma_region_t *root;
} vma_ctx_t;

vma_ctx_t *vma_alloc_ctx(pagemap_t pm, uint64_t start);
void      *vma_alloc(vma_ctx_t *ctx, size_t length, uint64_t flags);
void       vma_free(vma_ctx_t *ctx, void *addr);
void       vma_free_ctx (vma_ctx_t *ctx);