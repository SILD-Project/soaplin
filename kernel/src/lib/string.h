#pragma once
#include <stddef.h>

int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
char *strchr(const char *s, int c);
char *strcpy(char *dest, const char *src);
char *strrchr(const char *s, int c);
int oct2bin(unsigned char *str, int size);
char *strdup(const char *str);
char *strncpy(char *dest, const char *src, size_t n);