/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  idt.c - x86_64 Interrupt Descriptor Table implementation.
 */

#if defined(__x86_64__)

#include <stdint.h>

#include <arch/cpu.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/smp.h>
#include <arch/x86_64/ioapic.h>
#include "arch/x86_64/lapic.h"
#include <lib/log.h>

__attribute__((aligned(0x10))) 
static idt_entry_t idt[256];
static idtr_t idtr;

static uint8_t __idt_vectors[256];
static interrupt_handler __idt_external_handlers[256];

void idt_register_handler(uint8_t vector, void *isr) {
  if (vector <= 16)
    ioapic_redir_irq(bootstrap_lapic_id, vector + 32, vector, false);

  __idt_external_handlers[vector] = isr;
  __idt_vectors[vector <= 16 ? vector + 32 : vector] = vector <= 16 ? VT_HWI : VT_SWI;
}

void idt_interrupt_handler(registers_t *regs) {
  if (__idt_vectors[regs->int_no] == VT_SPURIOUS || __idt_vectors[regs->int_no] == VT_NONE)
    return;

  if (regs->int_no < 32) {
    fatal("Kernel panic: CPU exception %d\n", regs->int_no);
    fatal("rax: %p, rbx: %p, rbp: %p, rdx\n", regs->rax, regs->rbx, regs->rbp, regs->rdx);
    fatal("rdi: %p, rsi: %p, rcx: %p\n", regs->rdi, regs->rsi, regs->rcx);
    fatal("r8: %p, r9: %p, r10: %p\n", regs->r8, regs->r8, regs->r10);
    fatal("r11: %p, r12: %p, r13: %p\n", regs->r11, regs->r12, regs->r13);
    fatal("r14: %p, r15: %p\n", regs->r14, regs->r15);
    fatal("rip: %p, cs: %p, ss: %p\n", regs->rip, regs->cs, regs->ss);
    fatal("rflags: %p, err: %d, rsp: %p\n", regs->rflags, regs->err_code, regs->rsp);
    hcf();
  }

  int vec = regs->int_no;
  if (vec >= 32 && vec < 48)
    vec -= 32;

  if (__idt_vectors[regs->int_no] == VT_HWI) {
    interrupt_handler i = __idt_external_handlers[vec];
    i(regs);
  } else if (__idt_vectors[regs->int_no] == VT_SWI) {
    interrupt_handler i = __idt_external_handlers[regs->int_no];
    i(regs);
  }

  lapic_eoi();
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
  idt_entry_t* descriptor = &idt[vector];

  descriptor->isr_low        = (uint64_t)isr & 0xFFFF;
  descriptor->kernel_cs      = 0x08;
  descriptor->ist            = 0;
  descriptor->attributes     = flags;
  descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
  descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
  descriptor->reserved       = 0;
}


extern void* isr_stub_table[];

void idt_load() {
  __asm__ volatile ("lidt %0" : : "m"(idtr));
}

void idt_init() {
  idtr.base = (uintptr_t)&idt[0];
  idtr.limit = (uint16_t)sizeof(idt_entry_t) * 256 - 1;

  for (uint8_t vector = 0; vector < 32; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    __idt_vectors[vector] = VT_EXCEPTION;
  }
  trace("idt: Exception vectors has been set!\n");

  for (uint8_t vector = 32; vector < 48; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    __idt_vectors[vector] = VT_HWI;
  }
  trace("idt: Hardware interrupt vectors has been set!\n");
  for (uint16_t vector = 48; vector < 256; vector++) {
    idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
    __idt_vectors[vector] = VT_NONE;
  }
  __idt_vectors[IDT_SPURIOUS_INT] = VT_SPURIOUS;
  trace("idt: Spurious interrupt vector has been set!\n");

  //__asm__ volatile ("sti");
  idt_load();
  trace("arch: IDT loaded successfully\n");
}

#endif