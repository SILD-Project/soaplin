#include "sys/arch/x86_64/io.h"
#include "sys/gfx/flanterm/flanterm.h"
#include <lib/spinlock.h>
#include <lib/string.h>
#include <stdarg.h>
#include <sys/printf.h>

extern struct flanterm_context *ft_ctx;

static spinlock_t log_lock = {0};

void log(char *format, ...) {
  // spinlock_acquire(&log_lock);

  // TODO: replace this call with a call to printf() when the RTC is
  // implemented.
  char *date = "1970-01-01 00:00:00 | ";
  if (ft_ctx)
    flanterm_write(ft_ctx, date, strlen(date));

  char buf[2048];
  va_list l;
  va_start(l, format);
  npf_vsnprintf(buf, 2048, format, l);
  va_end(l);

  if (ft_ctx)
    flanterm_write(ft_ctx, buf, strlen(buf));

  /*for (int i = 0;; i++) {
    if (date[i] == '\0')
      break;

    outb(0xE9, date[i]);
  }

  for (int i = 0;; i++) {
    if (buf[i] == '\0')
      break;

    outb(0xE9, buf[i]);
  }*/

  // spinlock_release(&log_lock);
}