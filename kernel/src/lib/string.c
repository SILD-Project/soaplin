#include <lib/string.h>

size_t strlen(char *str) {
    size_t i = 0;
    while (str[i] != '\0')
        i++;
    return i;
}