/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  pmm.c - Physical memory allocator
 */

#include "arch/cpu.h"
#include <stdint.h>
#include <stddef.h>

#include <boot/limine.h>
#include <lib/log.h>
#include <mm/memop.h>
#include <mm/pmm.h>

uint64_t pmm_available_pages = 0;
uint64_t pmm_total_pages = 0;

static pmm_region_t *pmm_region_list_head = NULL;
static pmm_page_t *pmm_free_list_head = NULL;

void pmm_free_page(void *mem) {
    if (!mem)
        return;

    pmm_page_t *page = (pmm_page_t*)higher_half((uint64_t)mem);
    page->next = pmm_free_list_head ? (pmm_page_t*)higher_half((uint64_t)pmm_free_list_head) : 0x0;
    pmm_free_list_head = page;

    pmm_available_pages++;
}

static void __pmm_steal_pages_from_region_head(int pages) {
    for (int i = 0; i < pages; i++) {
        pmm_region_list_head->length -= PMM_PAGE_SIZE;
        void *page = (void*)pmm_region_list_head->base +
                            pmm_region_list_head->length;
        pmm_free_page(page);
        //trace("pmm: stealer: page is %p\n", page);

        if (pmm_region_list_head->length == 0)
        {
            // If a region is totally consumed,
            // we can turn it into a free page :)
            // So our 4kb aren't really lost
            void *mem = (void*)pmm_region_list_head;
            pmm_region_list_head = pmm_region_list_head->next;
            
            pmm_free_page(mem);
        }
    }
}

void *pmm_alloc_page() {
    if (!pmm_free_list_head)
    {
        if (!pmm_region_list_head) {
            fatal("pmm: out of memory!\n");
            hcf();
        }
        __pmm_steal_pages_from_region_head(4);
        // et voila, we now have 4 free pages to allocate
    }

    pmm_available_pages--;

    pmm_page_t *page = pmm_free_list_head;
    pmm_free_list_head = page->next;

    memset(page, 0, PMM_PAGE_SIZE);
    return (void*)physical((uint64_t)page);
}

void pmm_init() {
    struct limine_memmap_response *mmap = limine_get_memmap();

    for (uint64_t i = 0; i < mmap->entry_count; i++) {
        struct limine_memmap_entry *entry = mmap->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE || 
            entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
        {
            trace("pmm: found a usable memory block: %p-%p\n", entry->base, entry->base + entry->length);

            uint64_t newlen = ALIGN_DOWN(entry->length, PMM_PAGE_SIZE);

            // Give a page to store the PMM region.
            // When the region is fully consumed, the
            // page is freed so that it can be used (i love recycling)
            pmm_region_t *reg = (pmm_region_t*)higher_half(entry->base);
            reg->base = entry->base + PMM_PAGE_SIZE;
            reg->length = newlen - PMM_PAGE_SIZE;
            reg->next = pmm_region_list_head;
            pmm_region_list_head = reg;

            pmm_available_pages += reg->length / PMM_PAGE_SIZE;
        }
    }

    trace("pmm: %d pages available\n", pmm_available_pages);
}