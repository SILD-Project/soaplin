#pragma once

#include "arch/x86_64/idt.h"
#include "exec/exec.h"
#include "mm/vmm.h"

#define SCHED_KERNEL_PROCESS 0 // A process that runs in kernel mode.
#define SCHED_USER_PROCESS                                                     \
  1 // A process that runs in userspace. The code MUST be mapped directly after
    // creating the process.

typedef enum { SCHED_RUNNING, SCHED_DIED, SCHED_EMPTY } sched_proc_type;

typedef struct _sched_process {
  uint64_t *stack_base;
  uint64_t *kernel_stack; // Kernel-mode stack used for "syscall"

  char name[128];
  int pid;
  int type;
  int flags;

  registers_t regs;
  pagemap_t *pm;

  uint64_t *stack_end;
  uint64_t *stack_base_physical;

  struct _sched_process *next;
} sched_process;

extern sched_process *curr_proc;
extern sched_process *proc_list;

// The idle process is ditched in favor of standby mode,
// which activates when there's nothing to run.
// extern sched_process *idle_process;

void sched_init();
sched_process *sched_create(char *name, uint64_t entry_point, pagemap_t *pm,
                            uint32_t flags);

sched_process *sched_from_program(program_t *prog);

void sched_exit(int exit_code);
void schedule(registers_t *regs);