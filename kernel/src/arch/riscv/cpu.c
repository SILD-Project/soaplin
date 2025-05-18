#if defined (__riscv)

#include <arch/cpu.h>

void hcf() {
    for (;;) {
        asm ("wfi");
    }
}

#endif