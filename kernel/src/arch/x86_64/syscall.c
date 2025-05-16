#include <arch/x86_64/msr.h>
#include <arch/x86_64/syscall.h>
#include <stdint.h>

extern void syscall_entry();

void __x86_64_syscall_init() {
  uint64_t efer = rdmsr(IA32_EFER);
  efer |= (1 << 0);
  wrmsr(IA32_EFER, efer);
  uint64_t star = 0;
  star |= ((uint64_t)0x28 << 32); // kernel cs
  star |= ((uint64_t)0x30
           << 48); // user cs base (SYSCALL adds 16 for CS=0x38, 24 for SS=0x40)
  wrmsr(IA32_STAR, star);
  wrmsr(IA32_LSTAR, (uint64_t)syscall_entry);
  wrmsr(IA32_CSTAR, 0x0);
  wrmsr(IA32_CSTAR + 1, 0x200);
}