#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <arch/cpu.h>
#include <boot/limine.h>
#include <dev/tty.h>

void kmain(void) {
    tty_init();

    tty_write_raw("Hello, World!\n");

    // We're done, just hang...
    hcf();
}
