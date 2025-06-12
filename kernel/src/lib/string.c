/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  string.c - String manipulation functions implementation.
 */

#include <lib/string.h>

size_t strlen(const char *str) {
    size_t i = 0;
    while (str[i] != '\0')
        i++;
    return i;
}