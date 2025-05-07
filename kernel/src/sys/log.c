#include "rt.h"
#include "sys/arch/x86_64/io.h"
#include <stdarg.h>
#include <sys/printf.h>

void log(char *format, ...) {
    // TODO: replace this call with a call to printf() when the RTC is implemented.
    rt_print("1970-01-01 00:00:00 | ");

    char buf[2048];
    va_list l;
    va_start(l, format);
    npf_vsnprintf(buf, 2048, format, l);
    va_end(l);

    rt_print(buf);

    char *date = "1970-01-01 00:00:00 | ";
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