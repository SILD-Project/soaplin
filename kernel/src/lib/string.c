#include <lib/string.h>
#include <mm/liballoc/liballoc.h>
#include <mm/memop.h>
#include <stddef.h>

static char *olds;

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

#define DICT_LEN 256

static int *create_delim_dict(char *delim) {
  int *d = (int *)malloc(sizeof(int) * DICT_LEN);
  memset((void *)d, 0, sizeof(int) * DICT_LEN);

  int i;
  for (i = 0; i < strlen(delim); i++) {
    d[delim[i]] = 1;
  }
  return d;
}

char *strtok(char *str, char *delim) {

  static char *last, *to_free;
  int *deli_dict = create_delim_dict(delim);

  if (!deli_dict) {
    return NULL;
  }

  if (str) {
    last = (char *)malloc(strlen(str) + 1);
    if (!last) {
      free(deli_dict);
    }
    to_free = last;
    strcpy(last, str);
  }

  while (deli_dict[*last] && *last != '\0') {
    last++;
  }
  str = last;
  if (*last == '\0') {
    free(deli_dict);
    free(to_free);
    return NULL;
  }
  while (*last != '\0' && !deli_dict[*last]) {
    last++;
  }

  *last = '\0';
  last++;

  free(deli_dict);
  return str;
}
