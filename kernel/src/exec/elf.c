#include <exec/elf.h>
#include <stdint.h>
#include <sys/log.h>

void elf_load(char *data) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)data;
    if (   ehdr->e_ident[EI_MAG0] != ELFMAG0
        || ehdr->e_ident[EI_MAG1] != ELFMAG1
        || ehdr->e_ident[EI_MAG2] != ELFMAG2
        || ehdr->e_ident[EI_MAG3] != ELFMAG3)
    {
        log("elf - loading failed: magic is incorrect\n");
        return;
    }

    log("elf - e_ident[EI_DATA]: %d\n", ehdr->e_ident[EI_DATA]);
    log("elf - e_ident[EI_CLASS]: %d\n", ehdr->e_ident[EI_CLASS]);
    log("elf - e_ident[EI_OSABI]: %d\n", ehdr->e_ident[EI_OSABI]);
    log("elf - e_machine: %d\n", ehdr->e_machine);
    log("elf - e_entry: %p\n", ehdr->e_entry);
    log("elf - e_type: %p\n", ehdr->e_type);

    if (   ehdr->e_ident[EI_CLASS] != ELFCLASS64
        || ehdr->e_machine != EM_X86_64)
    {
        log("elf - loading failed: is the file built for amd64?\n");
        return;
    }

    if (ehdr->e_type != ET_EXEC)
    {
        log("elf - loading failed: ELF type isn't ET_EXEC\n");
        return;
    }

    Elf64_Phdr *phdr = (Elf64_Phdr*)data + ehdr->e_phoff;
    for (uint16_t i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD)
            continue;

        log("elf - Loadable program header: vaddr: %p", phdr[i].p_vaddr);
    }
}