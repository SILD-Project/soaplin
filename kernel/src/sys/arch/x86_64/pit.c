#include "sched/sched.h"
#include "sys/arch/x86_64/idt.h"
#include "sys/arch/x86_64/pic.h"
#include <sys/log.h>
#include <stdint.h>
#ifdef __x86_64__

#include <sys/arch/x86_64/pit.h>
#include <sys/arch/x86_64/idt.h>
//#include <sipaa/sched.h>

uint32_t tick = 0;

void pit_handler(registers_t *regs)
{
    tick++;

    schedule(regs);
    //Scheduler_Schedule(regs);
}

void pit_init(uint32_t frequency)
{
    uint32_t divisor = PIT_FREQUENCY / frequency;
    outb(0x43, 0x34);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));

    pic_unmask_irq(0);
}

void sleep(uint32_t seconds)
{
    uint32_t eticks = tick + seconds * HZ;
    while (tick < eticks)
    {
        __asm__ __volatile__("hlt");
    }
}

void sleep_ms(uint32_t milliseconds)
{
    uint32_t eticks = tick + (milliseconds * HZ) / 1000;
    while (tick < eticks)
    {
        __asm__ __volatile__("hlt");
    }
}

// todo: unistd: add usleep function
void usleep(uint32_t usecs)
{
    uint32_t eticks = tick + (usecs * HZ);
    while (tick < eticks)
    {
        __asm__ __volatile__("hlt");
    }
}

#endif