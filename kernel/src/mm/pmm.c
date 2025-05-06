#include "limine.h"
#include <stddef.h>
#include <mm/memop.h>
#include <mm/pmm.h>
#include <stdint.h>
#include <sys/log.h>

pmm_stack_t stack;
struct limine_memmap_response *_memmap;

uint64_t hhdm_offset = 0x0;

__attribute__((used, section(".limine_requests")))
struct limine_memmap_request mm_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 3
};


__attribute__((used, section(".limine_requests")))
struct limine_hhdm_request hhdm_req = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 3
};

int pmm_init()
{
    uint64_t free_pages = 0;
    hhdm_offset = hhdm_req.response->offset;

    struct limine_memmap_response *memmap = mm_req.response;
    _memmap = memmap;

    //DEBUG("mm", "----- PMM //INFO -----");
    int freemem = 0;
    for (uint64_t i = 0; i < memmap->entry_count; i++)
    {
        if (memmap->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            //DEBUG("mm", " - USABLE ENTRY\t\t@ 0x%.16llx, size: 0x%.16llx", memmap->entries[i]->base, memmap->entries[i]->length);
            free_pages += DIV_ROUND_UP(memmap->entries[i]->length, PMM_PAGE_SIZE);
            freemem += memmap->entries[i]->length;
        }
    }

    uint64_t array_size = ALIGN_UP(free_pages * 8, PMM_PAGE_SIZE);

    for (uint64_t i = 0; i < memmap->entry_count; i++)
    {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->length >= array_size && entry->type == LIMINE_MEMMAP_USABLE)
        {
            stack.pages = (uintptr_t*)HIGHER_HALF(entry->base);
            entry->length -= array_size;
            entry->base += array_size;
            //DEBUG("mm", " - STACK START\t\t@ 0x%.16llx", stack.pages);
            break;
        }
    }

    for (uint64_t i = 0; i < memmap->entry_count; i++)
    {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            for (uint64_t j = 0; j < entry->length; j += PMM_PAGE_SIZE)
            {
                stack.pages[stack.idx++] = entry->base + j;
            }
        }
    }

    stack.max = stack.idx;
    //DEBUG("mm", " - MAX INDEX:\t\t%d", stack.max);
    //DEBUG("mm", " - CURRENT INDEX:\t%d", stack.idx);
    //DEBUG("mm", "--------------------");


    log("pmm - %dmb is available to us.\n", freemem / (1024 * 1024));
    return 0;
}

void *pmm_request_page()
{
    if (stack.idx == 0)
    {
        //ERROR("mm", "No more pages available.");
        log("pmm - out of memory.\n");
        asm("cli");
        while (1) {
            asm("hlt");
        }
        return NULL;
    }

    uint64_t page_addr = stack.pages[--stack.idx];
    memset(HIGHER_HALF(page_addr), 0, PMM_PAGE_SIZE);
    return (void *)page_addr;
}

void pmm_free_page(void *ptr)
{
    if (ptr == NULL)
        return;

    if (stack.idx >= stack.max)
    {
        //ERROR("mm", "Stack overflow attempt while freeing a page.");
        log("pmm - could not free the page: stack overflow.\n");
        return;
    }

    stack.pages[stack.idx++] = (uint64_t)ptr;
}