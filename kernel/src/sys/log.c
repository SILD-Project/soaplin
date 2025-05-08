#include "sys/arch/x86_64/io.h"
#include "sys/gfx/flanterm/flanterm.h"
#include <stdarg.h>
#include <sys/printf.h>

extern struct flanterm_context *ft_ctx;

void log(char *format, ...) {
    // TODO: replace this call with a call to printf() when the RTC is implemented.
    char *date = "1970-01-01 00:00:00 | ";
    int i2 = 0; for (i2; date[i2] != 0; i2++);;
    flanterm_write(ft_ctx, date, i2);

    char buf[2048];
    va_list l;
    va_start(l, format);
    npf_vsnprintf(buf, 2048, format, l);
    va_end(l);

    int i = 0; for (i; buf[i] != 0; i++);;
    flanterm_write(ft_ctx, buf, i);

    for (int i=0;;i++) {
        if (date[i] == '\0')
            break;
        
        outb(0xE9, date[i]);
    }

    for (int i=0;;i++) {
        if (buf[i] == '\0')
            break;
        
        outb(0xE9, buf[i]);
    }
}