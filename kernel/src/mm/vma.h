// credit to kevin for writing the vma
// go check his projects: https://github.com/kevinalavik

#pragma once

#include <mm/vmm.h>
#include <mm/pmm.h>
#include <stdint.h>

typedef struct vma_region
{
    uint64_t start;
    uint64_t size;
    uint64_t flags;
    struct vma_region *next;
    struct vma_region *prev;
} vma_region_t;

typedef struct vma_context
{
    pagemap_t *pagemap;
    vma_region_t *root;
} vma_context_t;

extern vma_context_t *kernel_vma_context;

vma_context_t *vma_create_context(pagemap_t *pagemap);
void vma_destroy_context(vma_context_t *ctx);
void *vma_alloc(vma_context_t *ctx, uint64_t size, uint64_t flags);
void vma_free(vma_context_t *ctx, void *ptr);
void vma_dump_context(vma_context_t *ctx);