#pragma once

/*
 * A program (or a script, if a shebang is found inside), literally.
 */
#include "mm/vmm.h"

#define EXEC_TYPE_ELF 0

typedef struct {
  // The path to the program/script that will be executed.
  char *path;

  // The pagemap where the program's code is loaded.
  pagemap_t *pm;

  // The path to the first instruction. This will be passed to the new process's
  // rip register.
  uint64_t entry;

  // The program type. Used to get additional, unneeded information out of a
  // program
  int type;

  // That is what Soaplin needs to know. Executable file loaders are encouraged
  // to extend this structure to include info such as the EHDR for the ELF
  // loader...
} program_t;