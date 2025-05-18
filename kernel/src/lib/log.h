/*
 *  The Soaplin Kernel
 *  Copyright (C) 2025 The SILD Project
 *
 *  log.h - Kernel logging interface.
 */

#pragma once

// Log levels in order of increasing severity
#include <stdarg.h>
#include <stdbool.h>

#define LOGLEVEL_TRACE    7
#define LOGLEVEL_DEBUG    6
#define LOGLEVEL_INFO     5
#define LOGLEVEL_NOTICE   4
#define LOGLEVEL_WARNING  3
#define LOGLEVEL_ERROR    2
#define LOGLEVEL_FATAL    1

typedef void(*log_output_func)(char *str, va_list *vl);

void log_init(int max_loglevel);
bool log_register_output(log_output_func fn);
void log(int loglevel, char *str, ...);

// Shortcuts to log
#define trace(str, ...)   log(LOGLEVEL_TRACE,  str, ##__VA_ARGS__)
#define debug(str, ...)   log(LOGLEVEL_DEBUG,  str, ##__VA_ARGS__)
#define info(str, ...)    log(LOGLEVEL_INFO,   str, ##__VA_ARGS__)
#define notice(str, ...)  log(LOGLEVEL_NOTICE, str, ##__VA_ARGS__)
#define warn(str, ...)    log(LOGLEVEL_WARNING, str, ##__VA_ARGS__)
#define error(str, ...)   log(LOGLEVEL_ERROR,  str, ##__VA_ARGS__)
#define fatal(str, ...)   log(LOGLEVEL_FATAL,  str, ##__VA_ARGS__)