/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  idt.h - x86_64 Interrupt Descriptor Table declarations.
 */
#pragma once

#include <stdint.h>

#define VT_NONE      0
#define VT_EXCEPTION 1
#define VT_HWI       2
#define VT_SWI       3
#define VT_SPURIOUS  4

#define IDT_SPURIOUS_INT 0xFF

typedef struct {
  uint64_t r15;
  uint64_t r14;
  uint64_t r13;
  uint64_t r12;
  uint64_t r11;
  uint64_t r10;
  uint64_t r9;
  uint64_t r8;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t rbp;
  uint64_t rbx;
  uint64_t rdx;
  uint64_t rcx;
  uint64_t rax;
  uint64_t int_no;
  uint64_t err_code;
  uint64_t rip;
  uint64_t cs;
  uint64_t rflags;
  uint64_t rsp;
  uint64_t ss;
} __attribute__((packed)) registers_t;

typedef void(*interrupt_handler)(registers_t*);

typedef struct {
	uint16_t    isr_low;
	uint16_t    kernel_cs;
	uint8_t	    ist;
	uint8_t     attributes;
	uint16_t    isr_mid;
	uint32_t    isr_high;
	uint32_t    reserved;
} __attribute__((packed)) idt_entry_t;

typedef struct {
	uint16_t	limit;
	uint64_t	base;
} __attribute__((packed)) idtr_t;

void idt_register_handler(uint8_t vector, void *isr);
void idt_init(void);