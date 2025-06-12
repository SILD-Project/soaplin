/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  pit.c - x86_64 Programmable Interval Timer implementation.
 */
#if defined(__x86_64__)

#include <arch/x86_64/io.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/lapic.h>
#include <lib/log.h>

void pit_handler(registers_t *reg) {
    //trace("pit: Interrupt from %d!\n", lapic_get_id());
}

void cpu_init_timer() {
    outb(0x43, 0x36);

    uint16_t div = (uint16_t)(1193180 / 1000);

    // Since the PIT uses 8-bit IO operations,
    // we need to split div in 2.
    outb(0x40, (uint8_t)div);
    outb(0x40, (uint8_t)(div >> 8));

    idt_register_handler(0, pit_handler);
}

#endif