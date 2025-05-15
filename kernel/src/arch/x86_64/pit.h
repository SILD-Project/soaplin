#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include <arch//x86_64/io.h>

extern uint32_t tick;

void pit_init();
void pit_sleep(uint32_t ms);

#endif