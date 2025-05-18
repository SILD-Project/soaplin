/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  idt.c - x86_64 Interrupt Descriptor Table implementation.
 */
#if defined(__x86_64__)

#include "arch/cpu.h"
#include "lib/log.h"
#include <arch/x86_64/idt.h>

__attribute__((aligned(0x10))) 
static idt_entry_t idt[256];
static idtr_t idtr;

void idt_interrupt_handler(registers_t *regs) {
    fatal("Kernel panic: CPU exception %d\n", regs->int_no);
    fatal("rax: %p, rbx: %p, rbp: %p, rdx\n", regs->rax, regs->rbx, regs->rbp, regs->rdx);
    fatal("rdi: %p, rsi: %p, rcx: %p\n", regs->rdi, regs->rsi, regs->rcx);
    fatal("r8: %p, r9: %p, r10: %p\n", regs->r8, regs->r8, regs->r10);
    fatal("r11: %p, r12: %p, r13: %p\n", regs->r11, regs->r12, regs->r13);
    fatal("r14: %p, r15: %p\n", regs->r14, regs->r15);
    fatal("rip: %p, cs: %p, ss: %p\n", regs->rip, regs->cs, regs->ss);
    fatal("rflags: %p, err: %p, rsp: %p\n", regs->rflags, regs->err_code, regs->rsp);
    hcf();
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

static bool vectors[32];

extern void* isr_stub_table[];

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * 32 - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    __asm__ volatile ("sti");

    debug("arch: IDT loaded successfully\n");
}

#endif