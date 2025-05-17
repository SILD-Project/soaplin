// #include "sys/log.h"
#include "arch/x86_64/smp.h"
#include "dev/ioapic.h"
#include "dev/lapic.h"
#include "mm/vmm.h"
#include "sys/errhnd/panic.h"
#include <arch/x86_64/idt.h>
#include <stdbool.h>
#include <sys/log.h>
#include <sched/sched.h>

__attribute__((aligned(0x10))) static idt_entry_t idt[256];

static idtr_t idtr;
static int vectors[256];
extern void *isr_stub_table[];
idt_irq_handler_t irq_handler_table[256 - 32];

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
  idt_entry_t *descriptor = &idt[vector];

  descriptor->isr_low = (uint64_t)isr & 0xFFFF;
  descriptor->kernel_cs = 0x28;
  descriptor->ist = 0;
  descriptor->attributes = flags;
  descriptor->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
  descriptor->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
  descriptor->reserved = 0;
}

void idt_register_irq(uint8_t vector, void *isr) {
  if (vector <= 14)
    ioapic_redirect_irq(bootstrap_lapic_id, vector + 32, vector, false);

  irq_handler_table[vector] = isr;
  vectors[vector <= 14 ? vector + 32 : vector] = IDT_VT_IRQ;
}

void idt_init() {
  idtr.base = (uintptr_t)&idt[0];
  idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;

  for (uint16_t vector = 0; vector < 32; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    vectors[vector] = IDT_VT_EXCEPTION;
  }

  for (uint16_t vector = 32; vector < 256; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    vectors[vector] = IDT_VT_UNDEFINED;
  }

  // Do not use the legacy PIC.
  // pic_init();
  // pic_unmask_irq(1);
  // pic_unmask_irq(8);

  __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT
  __asm__ volatile("sti");                   // set the interrupt flag

  log("idt - initialized\n");
}

void idt_int_handler(registers_t *regs) {
  vmm_load_pagemap(vmm_kernel_pm);
  // log("kernel - Interrupt %d\n", regs->int_no);

  if (regs->int_no < 32) {
    if (regs->cs == 0x43) {
      log("ints - Process \"%s\" (pid: %d) caused a CPU fault: %d (err: %d, IP: %p)\n",
          curr_proc->name, curr_proc->pid, regs->int_no, regs->err_code, regs->rip);
      
      sched_exit(-144);
      return;
      //asm("cli");
      //while (1)
      //  asm("hlt");
    }

    panic_ctx("A CPU exception occured.", regs);
  }

  int vec = regs->int_no;
  if (vec >= 32 && vec < 47)
    vec -= 32;

  if (vectors[regs->int_no] == IDT_VT_IRQ) {
    idt_irq_handler_t i = irq_handler_table[vec];
    i(regs);
  }

  lapic_eoi();
  // pic_ack(regs->int_no - 32);
}