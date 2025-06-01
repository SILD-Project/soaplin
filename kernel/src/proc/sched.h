#pragma once

#include "arch/x86_64/idt.h"
#include "mm/paging.h"
#include <stddef.h>
#include <proc/exec/exec.h>

struct process;

typedef enum thread_state {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_TERMINATED
} thread_state_t;

typedef enum process_type {
    PT_USER,
    PT_KERNEL
} process_type_t;

typedef enum process_state {
    PS_ZOMBIE,
    PS_ACTIVE,
    PS_TERMINATED
} process_state_t;

typedef struct thread {
    size_t tid;
    registers_t regs;
    uint64_t stack_base;
    uint64_t stack_end;
    thread_state_t state;

    struct process *parent;

    // Local only: Only lists the process' threads
    struct thread  *next;
    struct thread  *prev; 
} thread_t;

// Global thread list
typedef struct thread_list {
    struct thread *next;
    struct thread *prev;
} thread_list_t;

typedef struct process {
    char *name;
    size_t pid;
    pagemap_t pm;
    thread_t *threads;
    exec_t *exec_clone;
    process_type_t type;
    process_state_t state;

    struct process *next_sibling;
    struct process *prev_sibling;

    struct process *children;
    struct process *parent;
} process_t;

extern thread_t *current_thread;
extern process_t *current_process;
extern thread_list_t *thread_list_head;

void sched_init();
void schedule();