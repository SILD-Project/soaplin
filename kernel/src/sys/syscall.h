#pragma once

#include "sys/arch/x86_64/idt.h"
#include <stdint.h>

/// A function that defines a system call.
/// NOTE: Arguments are defined as uint64_t, but you can simply put your own type, and then cast your function to syscall.
typedef uint64_t(*syscall)(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

/// Registers a system call.
/// NOTE: an existing system call cannot be replaced by another.
void syscall_register(int id, syscall handler);

/// Called by the interupt handler, or the "syscall" instruction handler
void syscall_handle(registers_t*);

/// Initialize the system calls.
void syscall_init();