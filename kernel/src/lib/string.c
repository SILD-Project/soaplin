#include <lib/string.h>
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

char *strcpy(char *dest, const char *src)
{
    if (dest == NULL || src == NULL)
        return NULL;
    
    char *temp = dest;
    while((*dest++ = *src++) != '\0'); 
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