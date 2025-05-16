#pragma once

#include "arch/x86_64/idt.h"

typedef struct __panic_backtrace {
    struct Idt_StackFrame* rbp;
    uint64_t rip;
} __attribute__((packed)) panic_backtrace_t;

void panic(char *msg);
void panic_ctx(char *msg, registers_t *regs);