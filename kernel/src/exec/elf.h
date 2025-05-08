#pragma once

#include <stdint.h>

// ELF magic.
#define EI_MAG0		 0
#define ELFMAG0	 	 0x7f

#define EI_MAG1		 1
#define ELFMAG1      'E'

#define EI_MAG2		 2
#define ELFMAG2		 'L'

#define EI_MAG3		 3
#define ELFMAG3		 'F'

// ELF class
#define EI_CLASS     4
#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2

// Is processor-specific data little-endian or big-endian?
#define EI_DATA      5
#define ELFDATANONE  0
#define ELFDATA2LSB  1
#define ELFDATA2MSB  2

// ELF version
#define EI_VERSION   6
#define EV_NONE      0
#define EV_CURRENT   1

// ELF ABI
#define EI_OSABI     7
#define ELFOSABI_NONE      0
#define ELFOSABI_SYSV   0
#define ELFOSABI_HPUX      1
#define ELFOSABI_NETBSD   2
#define ELFOSABI_LINUX      3
#define ELFOSABI_SOLARIS   6
#define ELFOSABI_IRIX      8
#define ELFOSABI_FREEBSD   9
#define ELFOSABI_TRU64   10
#define ELFOSABI_ARM      97
#define ELFOSABI_STANDALONE   255

// ABI version
#define EI_ABIVERSION 8

// Unused bytes.
#define EI_PAD 9

// Magic size.
#define EI_NIDENT 16

// e_type
#define ET_NONE		0
#define ET_REL		1
#define ET_EXEC		2
#define ET_DYN		3
#define ET_CORE		4

// e_machine (we only included machines supported by Soaplin.)
#define EM_X86_64	62

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Section;
typedef uint16_t Elf64_Versym;
typedef uint8_t Elf_Byte;
typedef uint16_t Elf64_Half;       
typedef int32_t Elf64_Sword;      
typedef uint32_t Elf64_Word;       
typedef int64_t Elf64_Sxword;     
typedef uint64_t Elf64_Xword;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    Elf64_Addr     e_entry;
    Elf64_Off      e_phoff;
    Elf64_Off      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} Elf64_Ehdr;

#define	PT_NULL		0
#define PT_LOAD		1
#define PT_DYNAMIC	2
#define PT_INTERP	3
#define PT_NOTE		4
#define PT_SHLIB	5

#define PF_X		(1 << 0)
#define PF_W		(1 << 1)
#define PF_R		(1 << 2)

typedef struct {
    uint32_t   p_type;
    uint32_t   p_flags;
    Elf64_Off  p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    uint64_t   p_filesz;
    uint64_t   p_memsz;
    uint64_t   p_align;
} Elf64_Phdr;

void elf_load(char *data);