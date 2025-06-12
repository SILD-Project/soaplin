/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  string.h - String manipulation functions.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *str);
uint32_t strtoul(const char *str, unsigned int len);
