/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  logoutputs_sk.c - Output functions for the logger
 */
#include "deps/npf.h"
#include <dev/tty.h>
#include <lib/logoutputs_sk.h>

void sklogoutput_tty(char *str, va_list *vl) {
    if (!vl) {
        tty_puts(str);
        return;
    }

    char buf[2048];
    npf_vsnprintf(buf, 2048, str, *vl);
    tty_puts(buf);
}

void sklogoutput_e9(char *str, va_list *vl) {
    // TODO: implement this
    (void)str;
    (void)vl;
}