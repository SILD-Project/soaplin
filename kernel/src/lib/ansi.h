/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  ansi.h - ANSI escape sequence generation functions.
 */

#pragma once
#include <stdint.h>

#define ANSI_CLEAR_SCREEN "\033[H\033[2J"
#define ANSI_RESET_COLOR  "\033[39m\\033[49m"

char *ansi_gen_bg(uint8_t r, uint8_t g, uint8_t b);
char *ansi_gen_fg(uint8_t r, uint8_t g, uint8_t b);