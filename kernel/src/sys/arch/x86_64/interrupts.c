//#include "mm/pmm.h"
//#include "mm/vmm.h"
#include "mm/vmm.h"
#include "sched/sched.h"
#include "sys/arch/x86_64/pic.h"
#include "sys/arch/x86_64/rtc.h"
#include "sys/log.h"
//#include "sys/sched.h"
#include <stdint.h>
#include <sys/arch/x86_64/idt.h>
#include <sys/arch/x86_64/io.h>
//#include <sys/errhand/panic.h>

int pit_millis = 0;
int pit_secs = 0;

struct Idt_StackFrame {
    struct Idt_StackFrame* rbp;
    uint64_t rip;
}__attribute__((packed));

void dump_backtrace(registers_t *r)
{
    log("ints - backtrace : \n");
    struct Idt_StackFrame* frame = (struct Idt_StackFrame*)r->rbp;

    while (frame) {
        log("ints -   %s (ip: %p)\n", frame->rip);
        frame = frame->rbp;
    }
    log("ints -   <end of backtrace>\n");
}

void pit_handler(registers_t *regs);

void exception_handler(registers_t *regs) {
    vmm_load_pagemap(vmm_kernel_pm);
    
    if (regs->int_no < 32) {
        //panic(kmode_cpu_exception, regs);
        log("ints - %d (RIP: %p, ERR: %d)\n", regs->int_no, regs->rip, regs->err_code);
        dump_backtrace(regs);
        asm ("cli");
        while (1)
            asm ("hlt");
    }

    if (regs->int_no == 1 + 32)
    {
        if (inb(0x60) & 0x80)
        {
            pic_ack(regs->int_no - 32);
            return;
        }

        log("ints - keyboard\n");
    }
    else if (regs->int_no == 32 + 8) {
        rtc_handle_interrupt(regs);
    }
    else if (regs->int_no == 0x80 - 32 || regs->int_no == 32) {
        pit_handler(regs);
    }
    else if (regs->int_no == 0x80)
    {
        log("syscall - Hello World! Current process: %s", curr_proc->name);
    }
    //logln(info, "arch/ints", "Received interrupt %d\n", regs->int_no);
    pic_ack(regs->int_no - 32);
}