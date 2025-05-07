#include "sys/arch/x86_64/idt.h"
#include "sys/arch/x86_64/io.h"
#include <sys/arch/x86_64/rtc.h>
#include <sys/arch/x86_64/pic.h>
#include <sys/printf.h>

void rtc_init() {
    asm("cli");
    outb(0x70, 0x8A);
    outb(0x71, 0x20);
    asm("sti");

    asm("cli");			// disable interrupts
    outb(0x70, 0x8B);		// select register B, and disable NMI
    char prev=inb(0x71);	// read the current value of register B
    outb(0x70, 0x8B);		// set the index again (a read will reset the index to register D)
    outb(0x71, prev | 0x40);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    asm("sti");

    //pic_unmask_irq(8);
}

void rtc_handle_interrupt(registers_t *regs) {
    (void)regs;
    printf("RTC!\n");
}