#pragma once

#include "sys/arch/x86_64/idt.h"
#include <stdint.h>

/// A function that defines a system call.
/// NOTE: Arguments are defined as uint64_t, but you can simply put your own type, and then cast your function to syscall.
typedef uint64_t(*syscall)(uint64_t rsp, uint64_t rbp, uint64_t r12, uint64_t r13, uint64_t r14, uint64_t r15);

/// Registers a system call.
/// NOTE: an existing system call cannot be replaced by another.
void syscall_register(int id, syscall handler);

/// Called by the interupt handler, or the "syscall" instruction handler
void syscall_handle(registers_t*);

/// Initialize the system calls.
void syscall_init();