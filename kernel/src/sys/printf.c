// Copyright (C) 2024 Sipaa Projects
// This code is part of the Soaplin kernel and is licensed under the terms of
// the MIT License.
#include "sys/gfx/flanterm/flanterm.h"
#include <stdarg.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0

// Compile nanoprintf in this translation unit.
#define NANOPRINTF_IMPLEMENTATION
#include <sys/printf.h>

extern struct flanterm_context *ft_ctx;

void printf(char *format, ...) {
    char buf[2048];
    va_list lst;
    va_start(lst, format);
    npf_vsnprintf(buf, 2048, format, lst);
    va_end(lst);

    //rt_print(buf);
    int i = 0; for (i; buf[i] != 0; i++);;
    flanterm_write(ft_ctx, buf, i);
}