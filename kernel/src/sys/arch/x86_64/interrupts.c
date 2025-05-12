#include "mm/pmm.h"
#include "mm/vmm.h"
#include "sched/sched.h"
#include "sys/arch/x86_64/pic.h"
#include "sys/arch/x86_64/rtc.h"
#include "sys/log.h"
#include "sys/syscall.h"
#include <stdint.h>
#include <sys/arch/x86_64/idt.h>
#include <sys/arch/x86_64/io.h>

int pit_millis = 0;
int pit_secs = 0;
extern int vmm_kernel_pm_exists;

void pit_handler(registers_t *regs);

void exception_handler(registers_t *regs) {
  vmm_load_pagemap(vmm_kernel_pm);

  if (regs->int_no < 32) {
    log("ints - %d (RIP: %p, ERR: %d)\n", regs->int_no, regs->rip,
        regs->err_code);

    if (regs->int_no == 0xe && vmm_kernel_pm_exists) {
      uint64_t cr2;
      asm("mov %%cr2, %0" : "=r"(cr2));
      log("ints - PF: Faulting location: %p (%p)\n", cr2,
          virt_to_phys(vmm_current_pm, cr2));
      log("ints - PF: Faulting page flags: %p\n",
          vmm_get_flags(vmm_current_pm, cr2));
      log("ints - PF: Faulting page map: %p\n", PHYSICAL(vmm_current_pm));
    }

    asm("cli");
    while (1)
      asm("hlt");
  }

  if (regs->int_no == 1 + 32) {
    if (inb(0x60) & 0x80) {
      pic_ack(regs->int_no - 32);
      return;
    }

    log("ints - keyboard\n");
  } else if (regs->int_no == 32 + 8) {
    log("rtc\n");
  } else if (regs->int_no == 0x80 - 32 || regs->int_no == 32) {
    pit_handler(regs);
  } else if (regs->int_no == 0x80) {
    syscall_handle(regs);
  }

  pic_ack(regs->int_no - 32);
}