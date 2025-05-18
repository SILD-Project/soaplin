#if defined (__loongarch64)

#include <arch/cpu.h>

void hcf() {
    for (;;) {
        asm ("idle 0");
    }
}

#endif