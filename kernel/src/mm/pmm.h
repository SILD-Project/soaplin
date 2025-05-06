#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#define PMM_PAGE_SIZE 4096

typedef struct pmm_stack
{
    uintptr_t *pages;
    uint64_t idx;
    uint64_t max;
} pmm_stack_t;

extern uint64_t hhdm_offset;

#define DIV_ROUND_UP(x, y) (((uint64_t)(x) + ((uint64_t)(y) - 1)) / (uint64_t)(y))
#define ALIGN_UP(x, y) (DIV_ROUND_UP(x, y) * (uint64_t)(y))
#define ALIGN_DOWN(x, y) (((uint64_t)(x) / (uint64_t)(y)) * (uint64_t)(y))

#define HIGHER_HALF(ptr) ((void *)((uint64_t)ptr) + hhdm_offset)
#define PHYSICAL(ptr) ((void *)((uint64_t)ptr) - hhdm_offset)

int pmm_init();
void *pmm_request_page();
void pmm_free_page(void *ptr);
void pmm_dump();

#endif // PMM_H