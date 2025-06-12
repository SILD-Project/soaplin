/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  vma.c - Virtual memory allocator
 */

#include <stddef.h>
#include <stdint.h>

#include "lib/log.h"
#include "mm/memop.h"
#include <mm/paging.h>
#include "mm/vma.h"
#include "mm/pmm.h"

vma_ctx_t *vma_kernel_ctx;

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

void *vma_realloc(vma_ctx_t *ctx, void *ptr, size_t length, uint64_t flags) {
    if (!ctx || !ptr) {
        return NULL;
    }

    if (length == 0)
    {
        vma_free(ctx, ptr);
        return NULL;
    }

    void *newmem = vma_alloc(ctx, length, flags);
    memcpy(newmem, ptr, length);
    vma_free(ctx, ptr);
    return newmem;
}

void *vma_alloc(vma_ctx_t *ctx, size_t length, uint64_t flags) {
    if (!ctx || !ctx->pm || !ctx->root)
        return NULL;

    vma_region_t *current = ctx->root;
    while (current) {
        if (current->next == NULL || current->start + (current->length * PMM_PAGE_SIZE) < current->next->start)
        {
            vma_region_t *reg = (vma_region_t*)higher_half((uint64_t)pmm_alloc_page());
            memset((void*)reg, 0, sizeof(vma_region_t));
            reg->start = current->start + (current->length * PMM_PAGE_SIZE);
            reg->length = length;
            reg->pflags = flags;
            reg->next = current->next;
            current->next = reg;

            // Now, allocate the memory
            for (uint64_t i = 0; i < length; i++) {
                uint64_t physical = (uint64_t)pmm_alloc_page();
                pg_map(ctx->pm, reg->start + (i * PMM_PAGE_SIZE), physical, flags);
            }

            return (void*)reg->start;
        }

        current = current->next;
    }
    return NULL;
}

static void __vma_free_region(vma_ctx_t *ctx, vma_region_t *target) {
    for (uint64_t i = 0; i < target->length; i++) {
        uint64_t vaddr = target->start + (i * PMM_PAGE_SIZE);
        uint64_t paddr = pg_physical(ctx->pm, vaddr);
        pg_unmap(ctx->pm, vaddr);
        pmm_free_page((void*)paddr);
    }

    pmm_free_page((void*)physical((uint64_t)target));
}

void vma_free(vma_ctx_t *ctx, void *addr) {
    if (!ctx || !ctx->pm || !ctx->root || !addr)
        return;

    vma_region_t *last = NULL;
    vma_region_t *target = ctx->root;

    while (target) {
        if (target->start == (uint64_t)addr)
            break;

        last   = target;
        target = target->next;
    }

    if (!target)
        return;

    if (last)
        last->next = target->next;
    else
        ctx->root = target->next;

    // Unallocate the memory
    __vma_free_region(ctx, target);
}

void vma_free_ctx(vma_ctx_t *ctx) {
    if (!ctx      ||
        !ctx->pm  ||
        !ctx->root)
        return;

    vma_region_t *reg = ctx->root;
    while (reg) {
        vma_region_t *next = reg->next;
        __vma_free_region(ctx, reg);
        reg = next;
    }

    pmm_free_page((void*)physical((uint64_t)ctx));
}