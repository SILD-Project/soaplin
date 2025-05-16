#include "arch/x86_64/idt.h"
#include "sched/sched.h"
#include <arch/x86_64/idt.h>
#include <arch/x86_64/pit.h>
#include <stdint.h>
#include <sys/log.h>

uint32_t tick = 0;

void pit_handler(registers_t *regs) {
  // log("PIT");
  tick++;
  schedule(regs);
}

void pit_init() {
  outb(0x43, 0x36);
  uint16_t div = (uint16_t)(1193180 / 1000);
  outb(0x40, (uint8_t)div);
  outb(0x40, (uint8_t)(div >> 8));
}

void pit_enable() { idt_register_irq(0, pit_handler); }

void pit_sleep(uint32_t ms) {
  uint64_t start = tick;
  while (tick - start < ms) {
    __asm__ volatile("nop");
  }
}