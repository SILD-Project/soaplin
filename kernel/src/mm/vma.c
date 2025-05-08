// credit to kevin for writing the vma
// go check his projects: https://github.com/kevinalavik

#include "mm/pmm.h"
#include "mm/vmm.h"
#include <mm/vma.h>
#include <mm/memop.h>
#include <sys/log.h>

vma_context_t *kernel_vma_context;

vma_context_t *vma_create_context(pagemap_t *pagemap)
{
    log("vma - creating VMA context with pagemap: 0x%.16llx\n", (uint64_t)pagemap);

    vma_context_t *ctx = (vma_context_t *)HIGHER_HALF(pmm_request_page());
    if (ctx == NULL)
    {
        log("vma - failed to allocate VMA context\n");
        return NULL;
    }
    log("vma - allocated VMA context at 0x%.16llx\n", (uint64_t)ctx);
    memset(ctx, 0, sizeof(vma_context_t));
    log("vma - zeroed out VMA context at 0x%.16llx\n", (uint64_t)ctx);

    ctx->root = (vma_region_t *)HIGHER_HALF(pmm_request_page());
    if (ctx->root == NULL)
    {
        log("vma - failed to allocate root region\n");
        pmm_free_page((void *)PHYSICAL(ctx));
        return NULL;
    }
    log("vma - allocated root region at 0x%.16llx\n", (uint64_t)ctx->root);

    ctx->pagemap = pagemap;
    ctx->root->start = PMM_PAGE_SIZE;
    ctx->root->size = 0;

    log("vma - VMA context created at 0x%.16llx with root region at 0x%.16llx\n", (uint64_t)ctx, (uint64_t)ctx->root);
    return ctx;
}

void vma_destroy_context(vma_context_t *ctx)
{
    log("vma - destroying VMA context at 0x%.16llx\n", (uint64_t)ctx);

    if (ctx->root == NULL || ctx->pagemap == NULL)
    {
        log("vma - invalid context or root passed to vma_destroy_context\n");
        return;
    }

    vma_region_t *region = ctx->root;
    while (region != NULL)
    {
        log("vma - freeing region at 0x%.16llx\n", (uint64_t)region);
        vma_region_t *next = region->next;
        pmm_free_page((void *)PHYSICAL(region));
        region = next;
    }

    pmm_free_page((void *)PHYSICAL(ctx));
    log("vma - destroyed VMA context at 0x%.16llx\n", (uint64_t)ctx);
}

void *vma_alloc(vma_context_t *ctx, uint64_t size, uint64_t flags)
{
    if (ctx == NULL || ctx->root == NULL || ctx->pagemap == NULL)
    {
        log("vma - invalid context or root passed to vma_alloc\n");
        return NULL;
    }

    vma_region_t *region = ctx->root;
    vma_region_t *new_region;
    vma_region_t *last_region = ctx->root;

    while (region != NULL)
    {
        if (region->next == NULL || region->start + region->size < region->next->start)
        {
            new_region = (vma_region_t *)HIGHER_HALF(pmm_request_page());
            if (new_region == NULL)
            {
                log("vma - failed to allocate new VMA region\n");
                return NULL;
            }

            memset(new_region, 0, sizeof(vma_region_t));
            new_region->size = size;
            new_region->flags = flags;
            new_region->start = region->start + region->size;
            new_region->next = region->next;
            new_region->prev = region;
            region->next = new_region;

            for (uint64_t i = 0; i < size; i++)
            {
                uint64_t page = (uint64_t)pmm_request_page();
                if (page == 0)
                {
                    log("vma - failed to allocate physical memory for VMA region\n");
                    return NULL;
                }

                vmm_map(ctx->pagemap, new_region->start + i * PMM_PAGE_SIZE, page, new_region->flags);
            }

            return (void *)new_region->start;
        }
        region = region->next;
    }

    new_region = (vma_region_t *)HIGHER_HALF(pmm_request_page());
    if (new_region == NULL)
    {
        log("vma - failed to allocate new VMA region\n");
        return NULL;
    }

    memset(new_region, 0, sizeof(vma_region_t));

    last_region->next = new_region;
    new_region->prev = last_region;
    new_region->start = last_region->start + last_region->size;
    new_region->size = size;
    new_region->flags = flags;
    new_region->next = NULL;

    for (uint64_t i = 0; i < size; i++)
    {
        uint64_t page = (uint64_t)pmm_request_page();
        if (page == 0)
        {
            log("vma - failed to allocate physical memory for VMA region\n");
            return NULL;
        }

        vmm_map(ctx->pagemap, new_region->start + i * PMM_PAGE_SIZE, page, new_region->flags);
    }

    return (void *)new_region->start;
}

void vma_free(vma_context_t *ctx, void *ptr)
{
    if (ctx == NULL)
    {
        log("vma - invalid context passed to vma_free\n");
        return;
    }

    vma_region_t *region = ctx->root;
    while (region != NULL)
    {
        if (region->start == (uint64_t)ptr)
        {
            log("vma - found region to free at 0x%.16llx\n", (uint64_t)region);
            break;
        }
        region = region->next;
    }

    if (region == NULL)
    {
        log("vma - unable to find region to free at address 0x%.16llx\n", (uint64_t)ptr);
        return;
    }

    vma_region_t *prev = region->prev;
    vma_region_t *next = region->next;

    for (uint64_t i = 0; i < region->size; i++)
    {
        uint64_t virt = region->start + i * PMM_PAGE_SIZE;
        uint64_t phys = virt_to_phys(vmm_kernel_pm, virt);

        if (phys != 0)
        {
            pmm_free_page((void *)phys);
            vmm_unmap(ctx->pagemap, virt);
        }
    }

    if (prev != NULL)
    {
        prev->next = next;
    }

    if (next != NULL)
    {
        next->prev = prev;
    }

    if (region == ctx->root)
    {
        ctx->root = next;
    }

    pmm_free_page((void *)PHYSICAL(region));
}
