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

static pmm_page_t* pmm_free_list_head = NULL;

void pmm_free_page(void *mem) {
    pmm_page_t *page = (pmm_page_t*)mem;
    page->next = pmm_free_list_head;
    pmm_free_list_head = page;

    pmm_available_pages++;
}

void *pmm_alloc_page() {
    if (!pmm_free_list_head)
    {
        fatal("pmm: out of memory!\n");
        hcf();
    }

    pmm_available_pages--;

    pmm_page_t *page = pmm_free_list_head;
    pmm_free_list_head = page->next;

    memset(page, 0, PMM_PAGE_SIZE);
    return page;
}

void pmm_init() {
    struct limine_memmap_response *mmap = limine_get_memmap();

    for (uint64_t i = 0; i < mmap->entry_count; i++) {
        struct limine_memmap_entry *entry = mmap->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            trace("pmm: found a usable memory block: %p-%p\n", entry->base, entry->base + entry->length);

            uint64_t newlen = ALIGN_UP(entry->length, PMM_PAGE_SIZE);
            for (uint64_t j = 0; j < newlen; j += PMM_PAGE_SIZE) {
                pmm_free_page((void*)(entry->base + j));
                pmm_total_pages++;
            }
        }
    }

    trace("pmm: %d pages available\n", pmm_available_pages);
}