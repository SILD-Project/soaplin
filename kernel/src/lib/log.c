/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  log.c - Kernel logging interface.
 */
#include <stdarg.h>
#include <lib/log.h>

static int __logger_max_loglevel = 0;
static log_output_func __logger_outputs[16];
static int __logger_output_count = 0;

static char* prelog[7] = {
    "\033[38;2;169;68;66;mFAULT  | \033[39m",
    "\033[38;2;217;83;79;mERROR  | \033[39m",
    "\033[38;2;240;173;78;mWARN   | \033[39m",
    "\033[38;2;240;240;240;mNOTICE | \033[39m",
    "\033[38;2;92;184;92;mINFO   | \033[39m",
    "\033[38;2;87;201;193;mDEBUG  | \033[39m",
    "\033[38;2;150;150;150;mTRACE  | \033[39m",
};

void log_init(int max_loglevel) {
    __logger_max_loglevel = max_loglevel;
}

bool log_register_output(log_output_func fn) {
    if (fn && __logger_output_count <= 16) {
        __logger_output_count ++;
        __logger_outputs[__logger_output_count - 1] = fn;
        return true;
    }
    return false;
}

void log(int loglevel, char *str, ...) {
    if (__logger_max_loglevel == 0 || __logger_output_count == 0)
        return; // The user forgot to initialize the logger.

    if (loglevel > __logger_max_loglevel)
        return; // The user does not want this type of log to show up.

    va_list vl;
    va_start(vl, str);

    for (int i = 0; i < __logger_output_count; i++) {
        __logger_outputs[i](prelog[loglevel - 1], (void*)0);
        __logger_outputs[i](str, &vl);
    }

    va_end(vl);
}