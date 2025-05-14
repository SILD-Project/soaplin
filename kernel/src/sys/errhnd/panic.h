#pragma once

#include "sys/arch/x86_64/idt.h"
void panic(char *msg);
void panic_ctx(char *msg, registers_t *regs);