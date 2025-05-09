#include "sched/sched.h"
#include <stdint.h>
#include <stddef.h>
#include <sys/log.h>
#include <sys/syscall.h>

static syscall syscall_table[1024];

static int syscall_initialized = 0;

// Stub function for undefined syscalls.
static uint64_t __syscall_undefined() { return 0; }

void syscall_handle(registers_t *regs) {
    if (regs->rax > 1024) {
        log("syscall - syscall_handle was called with rax better than what Soaplin supports (1024). did you forget to set rax?\n");
        return;
    }

    if (curr_proc == NULL || curr_proc->regs.cs != 0x43) {
        log("syscall - syscall_handle was called by the kernel. is this wanted?\n");
        return;
    }

    if (syscall_table[regs->rax] == (syscall)__syscall_undefined) {
        log("syscall - syscall_handle was called with an undefined system call. (%d)\n", regs->rax);
        return;
    }

    regs->rax = syscall_table[regs->rax](regs->rdi, regs->rsi, regs->rdx, regs->rcx, regs->r8, regs->r9);
    return;
}

void syscall_register(int id, syscall handler) {
    if (syscall_table[id] != (syscall)__syscall_undefined)
    {
        log("syscall - warning: syscall_register has been called to try replacing an existing syscall.\n");
        return;
    }

    syscall_table[id] = handler;
    log("syscall - System call %d has been set to %p", id, handler);
}

extern void syscall_exit(int exit_code);

void syscall_init() {
    for (int i = 0; i < 1024; i++)
        syscall_table[i] = (syscall)__syscall_undefined;

    syscall_register(0, (syscall)syscall_exit);
}