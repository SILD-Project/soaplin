#include "mm/liballoc/liballoc.h"
#include "mm/vmm.h"
#include <lib/spinlock.h>
#include <mm/vma.h>
#include <stddef.h>

extern vma_context_t *kernel_vma_context;

static spinlock_t liballoc_lock_var = {0};

int liballoc_lock() {
  spinlock_acquire(&liballoc_lock_var);
  return 0;
}

int liballoc_unlock() {
  spinlock_release(&liballoc_lock_var);
  return 0;
}

void *liballoc_alloc(size_t pages) {
  return vma_alloc(kernel_vma_context, pages, VMM_PRESENT | VMM_WRITABLE);
}

int liballoc_free(void *ptr, size_t pages) {
  (void)pages;
  vma_free(kernel_vma_context, ptr);
  return 0;
}

// void *malloc(size_t s) { return PREFIX(malloc)(s); }
// void *realloc(void *v, size_t s) { return PREFIX(realloc)(v, s); }
// void *calloc(size_t s1, size_t s) { return PREFIX(calloc)(s1, s); }
// void free(void *v) { return PREFIX(free)(v); }