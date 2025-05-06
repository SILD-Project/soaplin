#include "sched/sched.h"
#include "mm/pmm.h"
#include "mm/memop.h"
#include "mm/vmm.h"
#include "sys/arch/x86_64/idt.h"
#include "sys/log.h"
#include <stddef.h>

sched_process *proc_list;
sched_process *curr_proc;
int current_pid = 0;
int standby = 0;

void sched_init() {
    // TODO: It may be good to implement heap memory to save space.
    
    // We must initialize the process list.
    // By default, sched_create will append to this list.
    proc_list = pmm_request_page();
    memcpy(proc_list->name, "System\0", 7);
    proc_list->pid = -1;
    proc_list->type = SCHED_EMPTY;

    curr_proc = proc_list;

    standby = 1;
    log("sched - As there's nothing "
        "to schedule, the scheduler entered standby"
        "mode.\n");
}

sched_process *sched_create(char *name, uint64_t entry_point, uint32_t flags)
{
    // TODO: implement a separate strlen function
    // as there's like 4 strlen impls in the kernel.
    int i = 0;
    while (name[i] != 0)
        i++;

    sched_process *proc = pmm_request_page();
    memset(proc, 0, sizeof(sched_process));

    memcpy(proc->name, name, i);
    proc->pid = current_pid;
    proc->type = SCHED_RUNNING;

    // We are about to setup the registers ourself.
    // If it's broken, it's a boom in the ass of your computer
    // (and a CPU exception)

    proc->pm = vmm_alloc_pm();

    uint64_t *stack_phys = pmm_request_page();
    uint64_t *stack_virt = (uint64_t*)0x40000000;

    if (flags == SCHED_KERNEL_PROCESS) {
        proc->stack_base = stack_phys;
        proc->stack_end = proc_list->stack_base + PMM_PAGE_SIZE;
    } else if (flags == SCHED_USER_PROCESS) {
        vmm_map(proc->pm, (uint64_t)stack_virt, (uint64_t)stack_phys, VMM_PRESENT | VMM_WRITABLE | VMM_USER);
        proc->stack_base = stack_virt;
        proc->stack_end = proc_list->stack_base + PMM_PAGE_SIZE;
    }
    proc->regs.rip = (uint64_t)entry_point;

    if (flags == SCHED_KERNEL_PROCESS) {
        proc->regs.cs = 0x28; // Run in kernel mode
        proc->regs.ss = 0x30;
    }
    else if (flags == SCHED_USER_PROCESS) {
        proc->regs.cs = 0x43; // Run in kernel mode
        proc->regs.ss = 0x3B;
    }
    proc->regs.rflags = 0x202; // Enable interrupts
    proc->regs.rsp = (uint64_t)proc->stack_end;
    proc->regs.rbp = 0;

    proc->next = curr_proc->next;
    curr_proc->next = proc;

    current_pid++;

    if (standby) {
        // Disable standby mode as there's actually something to
        // run, now.
        standby = 0;
        log("sched - Standby mode has been"
        "disabled.\n");
    }
    return proc;
}

void schedule(registers_t *regs)
{
    if (standby) {
        //log("sched - Sched is in standby.\n");
        return;
    }

    memcpy(&curr_proc->regs, regs, sizeof(registers_t));

    curr_proc = curr_proc->next;
    if (curr_proc == NULL)
        curr_proc = proc_list;

    //log("sched - I choosed process %d\n", curr_proc->pid);
    memcpy(regs, &curr_proc->regs, sizeof(registers_t));

    // Finally, load our pagemap
    //vmm_load_pagemap(curr_proc->pm);
}