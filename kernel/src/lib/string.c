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

uint32_t strtoul(const char *str, unsigned int len) {
    unsigned int result = 0;
    for (unsigned int i = 0; i < len; i++) {
        result <<= 4;
        if (str[i] >= '0' && str[i] <= '9')
            result |= (str[i] - '0');
        else if (str[i] >= 'A' && str[i] <= 'F')
            result |= (str[i] - 'A' + 10);
        else if (str[i] >= 'a' && str[i] <= 'f')
            result |= (str[i] - 'a' + 10);
        else
            return 0;
    }
    return result;
}