#include <stddef.h>
#include <stdint.h>

void fpu_set_cw(const uint16_t cw) { asm volatile("fldcw %0" ::"m"(cw)); }

void fpu_activate() {
  size_t cr4;
  asm volatile("mov %%cr4, %0" : "=r"(cr4));
  cr4 |= 0x200;
  asm volatile("mov %0, %%cr4" ::"r"(cr4));
  fpu_set_cw(0x37F);
}