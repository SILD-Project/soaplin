#include <lib/string.h>
#include <mm/liballoc/liballoc.h>
#include <stddef.h>

int strlen(const char *str) {
  int len = 0;
  while (str[len])
    len++;
  return len;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

char *strcpy(char *dest, const char *src) {
  if (dest == NULL || src == NULL)
    return NULL;

  char *temp = dest;
  while ((*dest++ = *src++) != '\0')
    ;
  return temp;
}

char *strchr(const char *s, int c) {
  while (*s++) {
    if (*s == c)
      return (char *)s;
  }
  return NULL;
}

char *strrchr(const char *s, int c) {
  const char *p = NULL;

  for (;;) {
    if (*s == (char)c)
      p = s;
    if (*s++ == '\0')
      return (char *)p;
  }
}

int oct2bin(unsigned char *str, int size) {
  int n = 0;
  unsigned char *c = str;
  while (size-- > 0) {
    n *= 8;
    n += *c - '0';
    c++;
  }
  return n;
}

char *strdup(const char *str) {
  if (str == NULL)
    return NULL;

  int len = strlen(str);
  char *dup = (char *)malloc(len + 1);
  if (dup == NULL)
    return NULL;

  strcpy(dup, str);
  return dup;
}

char *strncpy(char *dest, const char *src, size_t n) {
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dest[i] = src[i];
  for (; i < n; i++)
    dest[i] = '\0';
  return dest;
}

