#include "mm/paging.h"
#include "mm/vma.h"
#include <mm/memop.h>
#include <proc/sched.h>
#include <stdint.h>

process_t *proc_list_head;
process_t *proc_list_tail;

thread_t *thread_list_head;
thread_t *thread_list_tail;
int numpid = 0;
int numtid = 0;

process_t *sched_new() {
    process_t *new = vma_alloc(vma_kernel_ctx, sizeof(process_t), PTE_PRESENT | PTE_WRITE);
    memset((void*)new, 0, sizeof(thread_t));
    
    new->pid = numpid++;
    new->pm = pg_alloc_pm();

    if (proc_list_head)
        proc_list_head->next = new;
    else
        proc_list_head = new;

    proc_list_tail = new;
    return new;
}

thread_t *sched_add_elf_thread(process_t *parent, char *elf) {
    thread_t *new = vma_alloc(vma_kernel_ctx, sizeof(process_t), PTE_PRESENT | PTE_WRITE);
    memset((void*)new, 0, sizeof(thread_t));

    new->tid = numtid++;
    new->parent = parent;
    

    if (thread_list_head)
        thread_list_head->next = new;
    else
        thread_list_head = new;

    thread_list_head = new;
}