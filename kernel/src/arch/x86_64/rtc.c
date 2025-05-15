#include "rtc.h"
#include "io.h"

char bcd;

unsigned char read_register(unsigned char reg) {
  __asm__ volatile("cli");
  outb(RTC_COMMAND, reg);
  return inb(RTC_DATA);
  __asm__ volatile("sti");
}

void write_register(unsigned char reg, unsigned char value) {
  __asm__ volatile("cli");
  outb(RTC_COMMAND, reg);
  outb(RTC_DATA, value);
  __asm__ volatile("sti");
}

unsigned char bcd2bin(unsigned char in_bcd) {
  return (bcd) ? ((in_bcd >> 4) * 10) + (in_bcd & 0x0F) : in_bcd;
}

int rtc_init() {
  __asm__ volatile("cli");
  unsigned char status;
  status = read_register(RTC_STATUS);
  status |= 0x02;         // 24 hour clock
  status |= 0x10;         // update ended interrupts
  status &= ~0x20;        // no alarm interrupts
  status &= ~0x40;        // no periodic interrupt
  bcd = !(status & 0x04); // check if data type is BCD
  write_register(RTC_STATUS, status);

  outb(0x70, 0x8B);      // select register B, and disable NMI
  char prev = inb(0x71); // read the current value of register B
  outb(0x70,
       0x8B); // set the index again (a read will reset the index to register D)
  outb(0x71, prev | 0x40); // write the previous value ORed with 0x40. This
                           // turns on bit 6 of register B
  __asm__ volatile("sti");
  return 0;
}