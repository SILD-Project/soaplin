#include "sys/arch/x86_64/idt.h"
#include <sys/log.h>

static registers_t __panic_regdump;

static void __panic_dump_regs() {
  asm volatile(
      // Save general purpose registers
      "movq %%r15, %0\n\t"
      "movq %%r14, %1\n\t"
      "movq %%r13, %2\n\t"
      "movq %%r12, %3\n\t"
      "movq %%r11, %4\n\t"
      "movq %%r10, %5\n\t"
      "movq %%r9,  %6\n\t"
      "movq %%r8,  %7\n\t"
      "movq %%rdi, %8\n\t"
      "movq %%rsi, %9\n\t"
      "movq %%rbp, %10\n\t"
      "movq %%rbx, %11\n\t"
      "movq %%rdx, %12\n\t"
      "movq %%rcx, %13\n\t"
      "movq %%rax, %14\n\t"

      // The rest of the registers (rip, cs, rflags, rsp, ss) would typically
      // be captured in the interrupt/exception handler context and passed in
      // We'll zero them out here since we don't have direct access
      : "=m"(__panic_regdump.r15), "=m"(__panic_regdump.r14),
        "=m"(__panic_regdump.r13), "=m"(__panic_regdump.r12),
        "=m"(__panic_regdump.r11), "=m"(__panic_regdump.r10),
        "=m"(__panic_regdump.r9), "=m"(__panic_regdump.r8),
        "=m"(__panic_regdump.rdi), "=m"(__panic_regdump.rsi),
        "=m"(__panic_regdump.rbp), "=m"(__panic_regdump.rbx),
        "=m"(__panic_regdump.rdx), "=m"(__panic_regdump.rcx),
        "=m"(__panic_regdump.rax)
      :
      : "memory");

  // Zero out the registers we can't directly access in this context
  __panic_regdump.int_no = 0;
  __panic_regdump.err_code = 0;
  __panic_regdump.rip = 0;
  __panic_regdump.cs = 0;
  __panic_regdump.rflags = 0;
  __panic_regdump.rsp = 0;
  __panic_regdump.ss = 0;
}

void panic() {
  __panic_dump_regs();

  log(" _  __                 _   ___           _          \n");
  log("| |/ /___ _ _ _ _  ___| | | _ \\__ _ _ _ (_)__       \n");
  log("| ' </ -_) '_| ' \\/ -_) | |  _/ _` | ' \\| / _|_ _ _ \n");
  log("|_|\\_\\___|_| |_||_\\___|_| |_| \\__,_|_||_|_\\__(_|_|_)\n");
  log("\n");
  log("Due to an error that can't be recovered from, Soaplin was needed to "
      "halt the PC.\n");
  log("Please report this error to the Soaplin developers, along with the "
      "information provided below:\n");
  log("-- REGISTER DUMP --\n");
  log("RDI: %p, RSI: %p, RDX: %p, RCX: %p, R8: %p, R9: %p\n",
      __panic_regdump.rdi, __panic_regdump.rsi, __panic_regdump.rdx,
      __panic_regdump.rcx, __panic_regdump.r8, __panic_regdump.r9);
  log("RAX: %p, RBP: %p, RBX: %p, R10: %p, R11: %p, R12: %p\n",
      __panic_regdump.rax, __panic_regdump.rbp, __panic_regdump.rbx,
      __panic_regdump.r10, __panic_regdump.r11, __panic_regdump.r12);
  log("R13: %p, R14: %p, R15: %p\n", __panic_regdump.r13, __panic_regdump.r14,
      __panic_regdump.r15);
  log("System halted.");

  asm("cli");
  for (;;)
    asm("hlt");
}