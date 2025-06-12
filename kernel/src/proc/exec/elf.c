#include "proc/exec/elf.h"
#include "mm/paging.h"
#include <lib/log.h>
#include <mm/memop.h>

Elf64_Ehdr *elf_load(char *dat, pagemap_t *pm) {
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)dat;
  Elf64_Phdr *phdrs = (Elf64_Phdr *)(dat + ehdr->e_phoff);

  if (memcmp(dat, "\177ELF", 4) != 0 || ehdr->e_machine != EM_X86_64) {
    error("elf: Invalid ELF at %p\n", dat);
    return NULL;
  }

  for (int i = 0; i < ehdr->e_phnum; i++) {
    Elf64_Phdr *ph = &phdrs[i];
    trace("elf: phdr: type: %d, vaddr: %p, memsz: %d, off: %d", ph->p_type,
          ph->p_vaddr, ph->p_memsz, ph->p_offset);
  }

  return ehdr;
}