/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  vma.c - Virtual memory allocator
 */

#include <stddef.h>
#include <stdint.h>

#include <mm/paging.h>
#include "mm/vma.h"
#include "mm/pmm.h"

vma_ctx_t *vma_alloc_ctx(pagemap_t pm, uint64_t start) {
    if (pm == NULL)
        return NULL;

    vma_ctx_t *ctx = (vma_ctx_t *)higher_half((uint64_t)pmm_alloc_page());
    ctx->pm = pm;
    
    vma_region_t *root_reg = (vma_region_t *)higher_half((uint64_t)pmm_alloc_page());
    root_reg->start = start;
    root_reg->length = 0;
    root_reg->next = NULL;

    ctx->root = root_reg;
    return ctx;
}

void vma_free_ctx(vma_ctx_t *ctx) {
    if (!ctx      ||
        !ctx->pm  ||
        !ctx->root)
        return;

    vma_region_t *reg = ctx->root;
    while (reg) {
        vma_region_t *next = reg->next;
        pmm_free_page((void*)physical((uint64_t)reg));
        reg = next;
    }

    pmm_free_page((void*)physical((uint64_t)ctx));
}