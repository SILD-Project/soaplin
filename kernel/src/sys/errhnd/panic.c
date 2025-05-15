#include "arch//x86_64/idt.h"
#include "lib/spinlock.h"
#include <mm/memop.h>
#include <sys/log.h>

static registers_t __panic_regdump;

static void __panic_dump_regs() {
  memset(&__panic_regdump, 0, sizeof(registers_t));

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

static void __panic_display_page_fault(registers_t *regs) {
    if (regs->int_no != 14) // 14 is the page fault interrupt number
        return;

    uint64_t cr2;
    asm volatile("mov %%cr2, %0" : "=r"(cr2));
    
    log("Page Fault Details:\n");
    log("Faulting Address (CR2): 0x%lx\n", cr2);
    log("Error Code: %d\n", regs->err_code);
    log("Flags:\n");
    if (!(regs->err_code & (1 << 0)))
        log("  - Page Not Present\n");
    else
        log("  - Protection Violation\n");
    
    if (regs->err_code & (1 << 1))
        log("  - Write Access\n");
    else
        log("  - Read Access\n");
    
    if (regs->err_code & (1 << 2))
        log("  - User-Mode Access\n");
    else
        log("  - Kernel-Mode Access\n");
    
    if (regs->err_code & (1 << 3))
        log("  - Reserved Bits Set\n");
    
    if (regs->err_code & (1 << 4))
        log("  - Instruction Fetch\n");
}

static void __panic_display_regs(registers_t *regs) {
  log("-- REGISTER DUMP --\n");
  log("RDI: %p, RSI: %p, RDX: %p, RCX: %p, R8: %p, R9: %p\n",
      regs->rdi, regs->rsi, regs->rdx,
      regs->rcx, regs->r8, regs->r9);
  log("RAX: %p, RBP: %p, RBX: %p, R10: %p, R11: %p, R12: %p\n",
      regs->rax, regs->rbp, regs->rbx,
      regs->r10, regs->r11, regs->r12);
  log("R13: %p, R14: %p, R15: %p\n", regs->r13, regs->r14,
      regs->r15);
  log("RIP: %p, CS: %x, SS: %x, RFLAGS: %d, INTERRUPT: %d, ERROR CODE: %d\n",
      regs->rip, regs->cs, regs->ss,
      regs->rflags, regs->int_no, regs->err_code);
  log("RSP: %p\n", regs->rsp);
  
  if (regs->int_no == 14) // If it's a page fault
    __panic_display_page_fault(regs);
}

void __panic_display_ascii_art() {
  log(" _  __                 _   ___           _          \n");
  log("| |/ /___ _ _ _ _  ___| | | _ \\__ _ _ _ (_)__       \n");
  log("| ' </ -_) '_| ' \\/ -_) | |  _/ _` | ' \\| / _|_ _ _ \n");
  log("|_|\\_\\___|_| |_||_\\___|_| |_| \\__,_|_||_|_\\__(_|_|_)\n");
  log("\n");
  log("Due to an error that can't be recovered from, Soaplin was needed to "
      "halt the PC.\n");
}

void panic(char *msg) {
  __panic_display_ascii_art();

  log("\n");
  log("%s\n", msg);
  log("\n");

  __panic_dump_regs();
  __panic_display_regs(&__panic_regdump);

  log("System halted: Please restart your computer manually.\n");

  asm("cli");
  for (;;)
    asm("hlt");
}

void panic_ctx(char *msg, registers_t *regs) {
  __panic_display_ascii_art();

  log("\n");
  log("%s\n", msg);
  log("\n");

  if (regs)
    __panic_display_regs(regs);
  else
    log("No register context provided.\n");


    
  log("System halted: Please restart your computer manually.\n");

  asm("cli");
  for (;;)
    asm("hlt");
}