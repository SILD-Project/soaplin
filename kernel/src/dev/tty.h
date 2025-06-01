/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  tty.h - Terminal interface declarations.
 */

#pragma once

void tty_init();
void tty_putc(char c);
void tty_puts(char *str);
void tty_printf(char *fmt, ...);