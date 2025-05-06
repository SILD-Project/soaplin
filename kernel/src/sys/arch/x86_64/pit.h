#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include <sys/arch/x86_64/io.h>

#define PIT_FREQUENCY 1193182
#define HZ 1000

extern uint32_t tick;

void pit_init(uint32_t frequency);
void sleep(uint32_t seconds);
void sleep_ms(uint32_t milliseconds);
void usleep(uint32_t usecs);

#endif