#ifndef PIT_H
#define PIT_H

#include <arch/x86_64/io.h>
#include <stdint.h>

extern uint32_t tick;

void pit_init();
void pit_sleep(uint32_t ms);
void pit_enable();

#endif