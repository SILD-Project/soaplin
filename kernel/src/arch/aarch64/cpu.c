#if defined (__aarch64__)

#include <arch/cpu.h>

void hcf() {
    for (;;) {
        asm ("wfi");
    }
}

#endif