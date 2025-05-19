/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  vmm.c - Virtual memory manager
 */

#include <stdbool.h>
#include <stddef.h>

#include <arch/cpu.h>
#include <boot/limine.h>
#include <exec/elf.h>
#include <lib/log.h>
#include <mm/memop.h>
#include <mm/pmm.h>
#include <mm/paging.h>
#include <stdint.h>

pagemap_t pg_kernel_pm = NULL;
pagemap_t pg_current_pm = NULL;

void pg_init() {
#if !defined(__x86_64__)
    fatal("vmm: not implemented\n");
    hcf();
#endif
    // Our objective here is to recreate the
    // kernel page map that Limine provide us

    pg_kernel_pm = pg_alloc_pm();

    uint64_t kvaddr = limine_get_kernel_vaddr();
    uint64_t kpaddr = limine_get_kernel_paddr();
    
    char *elf_addr = (char *)limine_get_kernel_ehdr_addr();
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_addr;

    for (uint16_t i = 0; i < ehdr->e_phnum; i++) {
        Elf64_Phdr *cur_phdr = (Elf64_Phdr*)(elf_addr + ehdr->e_phoff + (i * ehdr->e_phentsize));
        if (cur_phdr->p_type != PT_LOAD)
            continue;

        uintptr_t phys = (cur_phdr->p_vaddr - kvaddr) + kpaddr;
        uint64_t flags = PTE_PRESENT;
        if ((cur_phdr->p_flags & PF_X) == 0) {
            flags |= PTE_NX;
        }
        if (cur_phdr->p_flags & PF_W) {
            flags |= PTE_WRITE;
        }

        size_t length = ALIGN_UP(cur_phdr->p_memsz, PMM_PAGE_SIZE);
        
        for (uint64_t i = 0; i < length; i += PMM_PAGE_SIZE) {
            pg_map(pg_kernel_pm, cur_phdr->p_vaddr + i, phys + i, flags);
        }
        trace("vmm: Mapped range: %p -> %p (length: %x)\n", phys, cur_phdr->p_vaddr, length);
    }

    for (uint64_t i = 0; i < 0x100000000; i += PMM_PAGE_SIZE)
        pg_map(pg_kernel_pm, higher_half(i), i, PTE_PRESENT | PTE_WRITE);
    trace("vmm: Mapped range: %p -> %p (length: %x)\n", 0x0, 0xFFFF800000000000, 0x100000000);

    pg_load_pm(pg_kernel_pm);
    trace("vmm: Initialized.\n");
}

void pg_load_pm(pagemap_t pm) {
    if (!pm)
        return;

    pg_current_pm = pm;
    cpu_load_pm((pagemap_t)physical((uint64_t)pm));
}

pagemap_t pg_alloc_pm() {
    pagemap_t pm = (pagemap_t)higher_half((uint64_t)pmm_alloc_page());
    memset((void*)pm, 0, PMM_PAGE_SIZE);

    if (pg_kernel_pm)
    {
        for (int i = 256; i < 512; i++)
            pm[i] = pg_kernel_pm[i];
    }

    return pm;
}

void pg_free_pm(pagemap_t pm) {
    if (pm == pg_kernel_pm)
    {
        warn("vmm: Who tried to free the kernel's pagemap?!\n");
        return;
    }
    pmm_free_page((void*)pm);
}

static uint64_t *__pg_get_next_lvl(uint64_t *level, uint64_t entry,
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

void pg_map(pagemap_t pm, uint64_t vaddr, uint64_t paddr, uint64_t flags) {
  if (!pm) return;

  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __pg_get_next_lvl(pm  , pml4_entry, PTE_PRESENT | PTE_WRITE, true);
  uint64_t *pml2 = __pg_get_next_lvl(pml3, pml3_entry, PTE_PRESENT | PTE_WRITE, true);
  uint64_t *pml1 = __pg_get_next_lvl(pml2, pml2_entry, PTE_PRESENT | PTE_WRITE, true);

  pml1[pml1_entry] = paddr | flags;
}

void pg_map_user(pagemap_t pm, uint64_t vaddr, uint64_t paddr,
                  uint64_t flags) {
  if (!pm) return;

  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __pg_get_next_lvl(pm  , pml4_entry, flags, true);
  uint64_t *pml2 = __pg_get_next_lvl(pml3, pml3_entry, flags, true);
  uint64_t *pml1 = __pg_get_next_lvl(pml2, pml2_entry, flags, true);

  pml1[pml1_entry] = paddr | flags;
}

void pg_unmap(pagemap_t pm, uint64_t vaddr) {
  if (!pm) return;

  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __pg_get_next_lvl(pm  , pml4_entry, 0, false);
  if (!pml3) return;
  uint64_t *pml2 = __pg_get_next_lvl(pml3, pml3_entry, 0, false);
  if (!pml2) return;
  uint64_t *pml1 = __pg_get_next_lvl(pml2, pml2_entry, 0, false);
  if (!pml1) return;

  pml1[pml1_entry] = 0;
  cpu_invalidate_page(pm, vaddr);
}

void pg_protect(pagemap_t pm, uint64_t vaddr, uint64_t flags) {
  if (!pm) return;
  
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __pg_get_next_lvl(pm  , pml4_entry, 0, false);
  if (!pml3) return;
  uint64_t *pml2 = __pg_get_next_lvl(pml3, pml3_entry, 0, false);
  if (!pml2) return;
  uint64_t *pml1 = __pg_get_next_lvl(pml2, pml2_entry, 0, false);
  if (!pml1) return;

  uint64_t paddr = pml1[pml1_entry] & PTE_ADDR_MASK;
  pml1[pml1_entry] = paddr | flags;
}