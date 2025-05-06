#pragma once

#include "sys/arch/x86_64/idt.h"

void rtc_init();
void rtc_handle_interrupt(registers_t *regs);