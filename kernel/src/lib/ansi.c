/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  ansi.c - ANSI escape sequence generation functions implementation.
 */

#include <deps/npf.h>
#include <lib/ansi.h>
#include <stdint.h>

static char __ansi_bg_buf[32];
static char __ansi_fg_buf[32];

char *ansi_gen_bg(uint8_t r, uint8_t g, uint8_t b) {
    int len = npf_snprintf(__ansi_bg_buf, sizeof(__ansi_bg_buf), "\033[48;2;%d;%d;%d;m", r, g, b);
    if (len < 0 || len >= (int)sizeof(__ansi_bg_buf)) {
        return "\033[0m";
    }
    return __ansi_bg_buf;
}

char *ansi_gen_fg(uint8_t r, uint8_t g, uint8_t b) {
    int len = npf_snprintf(__ansi_fg_buf, sizeof(__ansi_fg_buf), "\033[38;2;%d;%d;%d;m", r, g, b);
    if (len < 0 || len >= (int)sizeof(__ansi_fg_buf)) {
        return "\033[0m";
    }
    return __ansi_fg_buf;
}