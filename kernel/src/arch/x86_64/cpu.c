#if defined (__x86_64__)

#include <arch/cpu.h>

void hcf() {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

#endif