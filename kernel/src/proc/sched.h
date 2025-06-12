#pragma once

#include "arch/x86_64/idt.h"
#include "mm/paging.h"
#include <stdint.h>

struct process;

typedef struct thread {
    uint64_t stack_base;
    registers_t regs;
    int tid;
    struct process *parent;
    struct thread  *next;
} thread_t;

typedef struct process {
    int pid;
    pagemap_t pm;
    struct process *next;
} process_t;

extern process_t *proc_list_head;
extern process_t *proc_list_tail;

extern thread_t *thread_list_head;
extern thread_t *thread_list_tail;

void sched_init();
process_t *sched_new();
void schedule();