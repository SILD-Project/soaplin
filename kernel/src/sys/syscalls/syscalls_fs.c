#include <stddef.h>
#include <sys/printf.h>

int syscall_write(int fd, const char *buf, size_t count) {
  if (fd == 1) {
    printf("%s", buf); // Prevent the buffer from being formatted.
    return count;
  }

  return 0;
}