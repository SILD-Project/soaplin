/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  vmm.c - Virtual memory manager
 */

#include "boot/limine.h"
#include "lib/log.h"
#include <stdbool.h>
#include <stddef.h>

#include <arch/cpu.h>
#include <mm/memop.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <stdint.h>

pagemap_t vmm_kernel_pm = NULL;
pagemap_t vmm_current_pm = NULL;

void vmm_init() {
#if !defined(__x86_64__)
    fatal("vmm: not implemented\n");
    hcf();
#endif
    // Our objective here is to recreate the
    // kernel page map that Limine provide us

    vmm_kernel_pm = vmm_alloc_pm();

    uint64_t kvaddr = limine_get_kernel_vaddr();
    uint64_t kpaddr = limine_get_kernel_paddr();
    uint64_t reqs_start = ALIGN_DOWN((uint64_t)reqs_start_ld, PMM_PAGE_SIZE);
    uint64_t reqs_end = ALIGN_UP((uint64_t)reqs_end_ld, PMM_PAGE_SIZE);
    uint64_t text_start = ALIGN_DOWN((uint64_t)text_start_ld, PMM_PAGE_SIZE);
    uint64_t text_end = ALIGN_UP((uint64_t)text_end_ld, PMM_PAGE_SIZE);
    uint64_t rodata_start = ALIGN_DOWN((uint64_t)rodata_start_ld, PMM_PAGE_SIZE);
    uint64_t rodata_end = ALIGN_UP((uint64_t)rodata_end_ld, PMM_PAGE_SIZE);
    uint64_t data_start = ALIGN_DOWN((uint64_t)data_start_ld, PMM_PAGE_SIZE);
    uint64_t data_end = ALIGN_UP((uint64_t)data_end_ld, PMM_PAGE_SIZE);

    // Now, map the kernel's sections
    for (uint64_t i = reqs_start; i < reqs_end; i += PMM_PAGE_SIZE)
        vmm_map(vmm_kernel_pm, i, i - kvaddr + kpaddr, PTE_PRESENT | PTE_WRITE); // why would i write into Limine requests?
    trace("vmm: Mapped limine rqs: PW\n");
    for (uint64_t i = text_start; i < text_end; i += PMM_PAGE_SIZE)
        vmm_map(vmm_kernel_pm, i, i - kvaddr + kpaddr, PTE_PRESENT);
    trace("vmm: Mapped text: P\n");
    for (uint64_t i = rodata_start; i < rodata_end; i += PMM_PAGE_SIZE)
        vmm_map(vmm_kernel_pm, i, i - kvaddr + kpaddr, PTE_PRESENT | PTE_NX);
    trace("vmm: Mapped rodata: P NX\n");
    for (uint64_t i = data_start; i < data_end; i += PMM_PAGE_SIZE)
        vmm_map(vmm_kernel_pm, i, i - kvaddr + kpaddr, PTE_PRESENT | PTE_WRITE | PTE_NX);
    trace("vmm: Mapped data: PW NX\n");

    // Map the lower 4 GiB into the higher-half
    for (uint64_t i = 0; i < 0x100000000; i += PMM_PAGE_SIZE)
        vmm_map(vmm_kernel_pm, higher_half(i), i, PTE_PRESENT | PTE_WRITE);
    trace("vmm: Mapped lower 4gib to higher half with flags: PW\n");

    cpu_load_pm(vmm_kernel_pm);

    trace("vmm: Initialized.\n");
}

void vmm_load_pm(pagemap_t pm) {
    if (!pm)
        return;

    vmm_current_pm = pm;
    cpu_load_pm((pagemap_t)physical((uint64_t)pm));
}

pagemap_t vmm_alloc_pm() {
    pagemap_t pm = (pagemap_t)higher_half((uint64_t)pmm_alloc_page());
    memset((void*)pm, 0, PMM_PAGE_SIZE);

    if (vmm_kernel_pm)
    {
        for (int i = 256; i < 512; i++)
            pm[i] = vmm_kernel_pm[i];
    }

    return pm;
}

void vmm_free_pm(pagemap_t pm) {
    if (pm == vmm_kernel_pm)
    {
        warn("vmm: Who tried to free the kernel's pagemap?!\n");
        return;
    }
    pmm_free_page((void*)pm);
}

static uint64_t *__vmm_get_next_lvl(uint64_t *level, uint64_t entry,
                                    uint64_t flags, bool alloc) {
  if (level[entry] & PTE_PRESENT)
    return (uint64_t *)higher_half(PTE_GET_ADDR(level[entry]));
  if (alloc) {
    uint64_t *pml = (uint64_t *)higher_half((uint64_t)pmm_alloc_page());
    memset(pml, 0, PMM_PAGE_SIZE);
    level[entry] = (uint64_t)physical((uint64_t)pml) | flags;
    return pml;
  }
  return NULL;
}

void vmm_map(pagemap_t pm, uint64_t vaddr, uint64_t paddr, uint64_t flags) {
  if (!pm) return;

  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm  , pml4_entry, PTE_PRESENT | PTE_WRITE, true);
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry, PTE_PRESENT | PTE_WRITE, true);
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry, PTE_PRESENT | PTE_WRITE, true);

  pml1[pml1_entry] = paddr | flags;
}

void vmm_map_user(pagemap_t pm, uint64_t vaddr, uint64_t paddr,
                  uint64_t flags) {
  if (!pm) return;

  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm  , pml4_entry, flags, true);
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry, flags, true);
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry, flags, true);

  pml1[pml1_entry] = paddr | flags;
}

void vmm_unmap(pagemap_t pm, uint64_t vaddr) {
  if (!pm) return;

  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm  , pml4_entry, 0, false);
  if (!pml3) return;
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry, 0, false);
  if (!pml2) return;
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry, 0, false);
  if (!pml1) return;

  pml1[pml1_entry] = 0;
  cpu_invalidate_page(pm, vaddr);
}

void vmm_protect(pagemap_t pm, uint64_t vaddr, uint64_t flags) {
  if (!pm) return;
  
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm  , pml4_entry, 0, false);
  if (!pml3) return;
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry, 0, false);
  if (!pml2) return;
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry, 0, false);
  if (!pml1) return;

  uint64_t paddr = pml1[pml1_entry] & PTE_ADDR_MASK;
  pml1[pml1_entry] = paddr | flags;
}