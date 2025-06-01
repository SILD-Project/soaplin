/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  logoutputs_sk.h - Output functions for the logger
 */

#pragma once

// Output to the terminal.
#include <stdarg.h>

void sklogoutput_tty(char *str, va_list *vl);

// Output to QEMU & Bochs's E9 port.
void sklogoutput_e9(char *str, va_list *vl);