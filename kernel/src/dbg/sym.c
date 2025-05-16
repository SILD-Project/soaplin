#include "dbg/sym.h"
#include "exec/elf.h"
#include "limine.h"
#include "sys/errhnd/panic.h"
#include "sys/log.h"
#include <lib/string.h>
#include <mm/liballoc/liballoc.h>

static Elf64_Sym *__ksym_symtab;
static char *__ksym_strtab;
static int __ksym_symcount = 0;

__attribute__((used, section(".limine_requests"))) static volatile struct
    limine_executable_file_request exec_file_rq = {
        .id = LIMINE_EXECUTABLE_FILE_REQUEST, .revision = 3};

void ksym_init() {
  char *img = exec_file_rq.response->executable_file->address;
  Elf64_Ehdr *ehdr = (Elf64_Ehdr *)img;
  if (ehdr->e_ident[EI_MAG0] != ELFMAG0 || ehdr->e_ident[EI_MAG1] != ELFMAG1 ||
      ehdr->e_ident[EI_MAG2] != ELFMAG2 || ehdr->e_ident[EI_MAG3] != ELFMAG3) {
    panic("kernel - how did i even boot? *starts galaxy brain meme*");
  }

  Elf64_Shdr *shdr = (Elf64_Shdr *)(img + ehdr->e_shoff);
  const char *shstrtab = (char *)img + shdr[ehdr->e_shstrndx].sh_offset;

  for (int j = 0; j < ehdr->e_shnum; j++) {
    if (shdr[j].sh_type == SHT_SYMTAB) {
      __ksym_symtab = (Elf64_Sym *)(img + shdr[j].sh_offset);
      __ksym_symcount = shdr[j].sh_size / shdr[j].sh_entsize;
    } else if (shdr[j].sh_type == SHT_STRTAB &&
               strcmp(shstrtab + shdr[j].sh_name, ".strtab") == 0) {
      __ksym_strtab = (char *)(img + shdr[j].sh_offset);
    }
  }

  log("kernel - loaded %d symbols\n", __ksym_symcount);
}

func *ksym_fromip(uint64_t ip) {
  for (int i = 0; i < __ksym_symcount; i++) {
    uint64_t value = __ksym_symtab[i].st_value;
    uint64_t size = __ksym_symtab[i].st_size;

    if (value <= ip && ip < (value + size)) {
      func *f = malloc(sizeof(func));
      f->base = value;
      f->ip = ip;
      f->name = (__ksym_strtab + __ksym_symtab[i].st_name);
      return f;
    }
  }

  return NULL;
}