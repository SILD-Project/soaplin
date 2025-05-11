#include "mm/vmm.h"
#include "limine.h"
#include "mm/memop.h"
#include "mm/pmm.h"
#include "sys/log.h"
#include "vmm.h"
#include <stdbool.h>
#include <stddef.h>

__attribute__((
    used,
    section(
        ".limine_requests"))) static volatile struct limine_paging_mode_request
    pmrq = {.id = LIMINE_PAGING_MODE_REQUEST,
            .revision = 3,
            .mode = LIMINE_PAGING_MODE_X86_64_4LVL};

__attribute__((used, section(".limine_requests"))) static volatile struct
    limine_executable_address_request karq = {
        .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
        .revision = 0,
};

pagemap_t *vmm_kernel_pm = NULL;
pagemap_t *vmm_current_pm = NULL;
int vmm_kernel_pm_exists = 0;

pagemap_t *vmm_alloc_pm() {
  pagemap_t *pm = (pagemap_t *)HIGHER_HALF((uint64_t)pmm_request_page());
  memset(pm, 0, PMM_PAGE_SIZE);

  pm->toplevel = (uint64_t *)HIGHER_HALF((uint64_t)pmm_request_page());
  memset(pm->toplevel, 0, PMM_PAGE_SIZE);

  if (vmm_kernel_pm_exists) {
    for (int i = 256; i < 512; i++) {
      pm->toplevel[i] = vmm_kernel_pm->toplevel[i];
    }
  }

  return pm;
}

void vmm_release_pm(pagemap_t *pm) {
  memset(pm->toplevel, 0, PMM_PAGE_SIZE);
  memset(pm, 0, PMM_PAGE_SIZE);
  pmm_free_page(pm->toplevel);
  pmm_free_page(pm);
}

void vmm_sanity_check() {
  uint64_t *my_memory = pmm_request_page();
  *my_memory = 0x40;

  pagemap_t *pm = vmm_alloc_pm();
  vmm_map(pm, 0x1000, (uint64_t)my_memory, VMM_PRESENT | VMM_WRITABLE);

  uint64_t *my_ptr = (uint64_t *)0x1000;
  uint64_t ptr_val = 0;
  vmm_load_pagemap(pm);
  ptr_val = *my_ptr;
  vmm_load_pagemap(vmm_kernel_pm);

  if (ptr_val != 0x40) {
    log("vmm - sanity check failed. system halted.\n");
    asm("cli");
    while (1)
      asm("hlt");
  }
}

void vmm_init() {
  if (pmrq.response->mode != LIMINE_PAGING_MODE_X86_64_4LVL) {
    log("vmm - Soaplin only supports 4-level paging!\n");
    asm("cli; hlt;");
  }
  vmm_kernel_pm = vmm_alloc_pm();
  vmm_kernel_pm_exists = 1;

  uint64_t kphysaddr = karq.response->physical_base;
  uint64_t kvirtaddr = karq.response->virtual_base;

  uint64_t reqs_start = ALIGN_DOWN((uint64_t)reqs_start_ld, PMM_PAGE_SIZE);
  uint64_t reqs_end = ALIGN_UP((uint64_t)reqs_end_ld, PMM_PAGE_SIZE);
  uint64_t text_start = ALIGN_DOWN((uint64_t)text_start_ld, PMM_PAGE_SIZE);
  uint64_t text_end = ALIGN_UP((uint64_t)text_end_ld, PMM_PAGE_SIZE);
  uint64_t rodata_start = ALIGN_DOWN((uint64_t)rodata_start_ld, PMM_PAGE_SIZE);
  uint64_t rodata_end = ALIGN_UP((uint64_t)rodata_end_ld, PMM_PAGE_SIZE);
  uint64_t data_start = ALIGN_DOWN((uint64_t)data_start_ld, PMM_PAGE_SIZE);
  uint64_t data_end = ALIGN_UP((uint64_t)data_end_ld, PMM_PAGE_SIZE);

  log("vmm - mapping .requests section...\n");
  for (uint64_t req = reqs_start; req < reqs_end; req += PMM_PAGE_SIZE) {
    vmm_map(vmm_kernel_pm, req, req - kvirtaddr + kphysaddr,
            VMM_PRESENT | VMM_WRITABLE);
  }

  log("vmm - mapping .text section...\n");
  for (uint64_t text = text_start; text < text_end; text += PMM_PAGE_SIZE) {
    vmm_map(vmm_kernel_pm, text, text - kvirtaddr + kphysaddr, VMM_PRESENT);
  }

  log("vmm - mapping .rodata section...\n");
  for (uint64_t roData = rodata_start; roData < rodata_end;
       roData += PMM_PAGE_SIZE)
    vmm_map(vmm_kernel_pm, roData, roData - kvirtaddr + kphysaddr,
            VMM_PRESENT | VMM_NX);

  log("vmm - mapping .data section...\n");
  for (uint64_t data = data_start; data < data_end; data += PMM_PAGE_SIZE)
    vmm_map(vmm_kernel_pm, data, data - kvirtaddr + kphysaddr,
            VMM_PRESENT | VMM_WRITABLE | VMM_NX);

  log("vmm - mapping address from 0x0 to 0x100000000...\n");
  for (uint64_t gb4 = 0; gb4 < 0x100000000; gb4 += PMM_PAGE_SIZE) {
    vmm_map(vmm_kernel_pm, gb4, gb4, VMM_PRESENT | VMM_WRITABLE);
    vmm_map(vmm_kernel_pm, (uint64_t)HIGHER_HALF(gb4), gb4,
            VMM_PRESENT | VMM_WRITABLE);
  }

  vmm_load_pagemap(vmm_kernel_pm);

  vmm_sanity_check();
  log("vmm - initialized!\n");
}

void vmm_load_pagemap(pagemap_t *pm) {
  if (!pm)
    return;

  if (!pm->toplevel)
    return;

  vmm_current_pm = pm;
  __asm__ volatile("mov %0, %%cr3" : : "r"(PHYSICAL(pm->toplevel)) : "memory");
}

static uint64_t *__vmm_get_next_lvl(uint64_t *level, uint64_t entry,
                                    uint64_t flags, bool alloc) {
  if (level[entry] & VMM_PRESENT)
    return HIGHER_HALF(PTE_GET_ADDR(level[entry]));
  if (alloc) {
    uint64_t *pml = (uint64_t *)HIGHER_HALF(pmm_request_page());
    memset(pml, 0, PMM_PAGE_SIZE);
    level[entry] = (uint64_t)PHYSICAL(pml) | flags;
    return pml;
  }
  return NULL;
}

uint64_t vmm_get_flags(pagemap_t *pm, uint64_t vaddr) {
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm->toplevel, pml4_entry, 0, false);
  if (!pml3)
    return 0;
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry, 0, false);
  if (!pml2)
    return 0;
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry, 0, false);
  if (!pml1)
    return 0;

  return pml1[pml1_entry] & 0x7000000000000FFF;
}

uint64_t virt_to_phys(pagemap_t *pagemap, uint64_t virt) {
  uint64_t pml4_idx = (virt >> 39) & 0x1FF;
  uint64_t pml3_idx = (virt >> 30) & 0x1FF;
  uint64_t pml2_idx = (virt >> 21) & 0x1FF;
  uint64_t pml1_idx = (virt >> 12) & 0x1FF;

  uint64_t *pml3 = __vmm_get_next_lvl(pagemap->toplevel, pml4_idx, 0, false);
  if (!pml3)
    return 0;
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_idx, 0, false);
  if (!pml2)
    return 0;
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_idx, 0, false);
  if (!pml1)
    return 0;

  uint64_t phys_addr = pml1[pml1_idx] & 0x000FFFFFFFFFF000;

  return phys_addr;
}

void vmm_map(pagemap_t *pm, uint64_t vaddr, uint64_t paddr, uint64_t flags) {
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm->toplevel, pml4_entry,
                                      VMM_PRESENT | VMM_WRITABLE, true);
  uint64_t *pml2 =
      __vmm_get_next_lvl(pml3, pml3_entry, VMM_PRESENT | VMM_WRITABLE, true);
  uint64_t *pml1 =
      __vmm_get_next_lvl(pml2, pml2_entry, VMM_PRESENT | VMM_WRITABLE, true);

  pml1[pml1_entry] = paddr | flags;
}

void vmm_map_user(pagemap_t *pm, uint64_t vaddr, uint64_t paddr,
                  uint64_t flags) {
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;

  uint64_t *pml3 =
      __vmm_get_next_lvl(pm->toplevel, pml4_entry, flags | VMM_WRITABLE,
                         true); // PML3 / Page Directory Pointer Entry
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry, flags | VMM_WRITABLE,
                                      true); // PML2 / Page Directory Entry
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry, flags | VMM_WRITABLE,
                                      true); // PML1 / Page Table Entry

  pml1[pml1_entry] = paddr | flags;
}

void vmm_unmap(pagemap_t *pm, uint64_t vaddr) {
  uint64_t pml1_entry = (vaddr >> 12) & 0x1ff;
  uint64_t pml2_entry = (vaddr >> 21) & 0x1ff;
  uint64_t pml3_entry = (vaddr >> 30) & 0x1ff;
  uint64_t pml4_entry = (vaddr >> 39) & 0x1ff;

  uint64_t *pml3 = __vmm_get_next_lvl(pm->toplevel, pml4_entry, 0, false);
  if (pml3 == NULL)
    return;
  uint64_t *pml2 = __vmm_get_next_lvl(pml3, pml3_entry, 0, false);
  if (pml2 == NULL)
    return;
  uint64_t *pml1 = __vmm_get_next_lvl(pml2, pml2_entry, 0, false);
  if (pml1 == NULL)
    return;

  pml1[pml1_entry] = 0;

  __asm__ volatile("invlpg (%0)" : : "b"(vaddr) : "memory");
}