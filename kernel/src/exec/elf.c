#include "exec/exec.h"
#include "mm/memop.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include <mm/liballoc/liballoc.h>
#include <exec/elf.h>
#include <stdint.h>
#include <sys/log.h>

program_t *elf_load(char *data) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)data;
    if (   ehdr->e_ident[EI_MAG0] != ELFMAG0
        || ehdr->e_ident[EI_MAG1] != ELFMAG1
        || ehdr->e_ident[EI_MAG2] != ELFMAG2
        || ehdr->e_ident[EI_MAG3] != ELFMAG3)
    {
        log("elf - loading failed: magic is incorrect\n");
        return NULL;
    }

    log("elf - e_ident[EI_DATA]: %d\n", ehdr->e_ident[EI_DATA]);
    log("elf - e_ident[EI_CLASS]: %d\n", ehdr->e_ident[EI_CLASS]);
    log("elf - e_ident[EI_OSABI]: %d\n", ehdr->e_ident[EI_OSABI]);
    log("elf - e_machine: %d\n", ehdr->e_machine);
    log("elf - e_entry: %p\n", ehdr->e_entry);
    log("elf - e_type: %p\n", ehdr->e_type);
    log("elf - e_phnum: %p\n", ehdr->e_phnum);

    if (   ehdr->e_ident[EI_CLASS] != ELFCLASS64
        || ehdr->e_machine != EM_X86_64)
    {
        log("elf - loading failed: is the file built for amd64?\n");
        return NULL;
    }

    if (ehdr->e_type != ET_EXEC)
    {
        log("elf - loading failed: ELF type isn't ET_EXEC\n");
        return NULL;
    }

    // There's the interesing part
    elf_program_t *ret = malloc(sizeof(elf_program_t)); // Allocate memory for the program.

    pagemap_t *pm = vmm_alloc_pm(); // Allocate a pagemap, so that we can map the program inside.
    ret->program.pm = pm;
    ret->program.entry = ehdr->e_entry;
    ret->ehdr = ehdr;

    Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)data + ehdr->e_phoff);

    for (uint16_t i = 0; i <= ehdr->e_phnum; i++)
    {
        log("elf - ELF segment type: %d\n", phdr[i].p_type);
        if (phdr[i].p_type != PT_LOAD)
            continue;

        uint64_t vaddr_start = ALIGN_DOWN(phdr[i].p_vaddr, PMM_PAGE_SIZE);
        uint64_t vaddr_end = ALIGN_UP(phdr[i].p_vaddr + phdr[i].p_memsz, PMM_PAGE_SIZE);
        uint64_t offset = phdr[i].p_offset;

        uint64_t flags = VMM_PRESENT;
        if (phdr[i].p_flags & PF_W)
            flags |= VMM_WRITABLE;
        if (!(phdr[i].p_flags & PF_X))
            flags |= VMM_NX;

        flags |= VMM_USER; // User mode access

        log("elf - loading ELF program header %u: vaddr 0x%llx - 0x%llx, offset 0x%llx, filesz 0x%llx, size 0x%llx, flags 0x%llx\n",
              i, vaddr_start, vaddr_end, offset, phdr[i].p_filesz, phdr[i].p_memsz, flags);

        uint64_t page_offset = phdr[i].p_vaddr & (PMM_PAGE_SIZE - 1);

        for (uint64_t vaddr = vaddr_start; vaddr < vaddr_end; vaddr += PMM_PAGE_SIZE)
        {
            uint64_t phys = (uint64_t)pmm_request_page();
            if (!phys)
            {
                log("elf - pmm page alloc failed. out of memory?\n");
                return 0;
            }

            vmm_map(pm, vaddr, phys, flags);
            memset((void *)HIGHER_HALF(phys), 0, PMM_PAGE_SIZE);

            uint64_t file_page_offset = offset + (vaddr - vaddr_start);
            uint64_t file_data_end = offset + phdr[i].p_filesz;

            if (file_page_offset < file_data_end)
            {
                uint64_t bytes_from_start = vaddr - vaddr_start;
                uint64_t page_data_offset = 0;

                if (bytes_from_start == 0 && page_offset > 0)
                {
                    page_data_offset = page_offset;
                }

                uint64_t copy_offset = file_page_offset;
                uint64_t copy_size = PMM_PAGE_SIZE - page_data_offset;

                if (copy_offset + copy_size > file_data_end)
                {
                    copy_size = file_data_end - copy_offset;
                }

                if (copy_size > 0)
                {
                    void *dest = (void *)(HIGHER_HALF(phys) + page_data_offset);
                    void *src = (uint8_t *)data + copy_offset;
                    memcpy(dest, src, copy_size);

                    //log("elf - copied 0x%llx bytes from ELF file offset 0x%llx to vaddr 0x%llx (phys 0x%llx)\n",
                    //      copy_size, copy_offset, vaddr + page_data_offset, phys + page_data_offset);
                }
            }
        }
    }

    log("elf - loaded ELF program in memory.\n");
    return (program_t*)ret;
}