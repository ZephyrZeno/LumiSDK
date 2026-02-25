/**
 * log.c — LumiOS SDK logging subsystem
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static lumi_log_level_t g_min_level = LUMI_LOG_INFO;

static const char *level_str(lumi_log_level_t level) {
    switch (level) {
        case LUMI_LOG_VERBOSE: return "V";
        case LUMI_LOG_DEBUG:   return "D";
        case LUMI_LOG_INFO:    return "I";
        case LUMI_LOG_WARN:    return "W";
        case LUMI_LOG_ERROR:   return "E";
        default:               return "?";
    }
}

void lumi_log_set_level(lumi_log_level_t min_level) {
    g_min_level = min_level;
}

void lumi_log(lumi_log_level_t level, const char *tag, const char *fmt, ...) {
    if (level < g_min_level) return;

    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char timebuf[32];
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", tm);

    FILE *out = (level >= LUMI_LOG_WARN) ? stderr : stdout;
    fprintf(out, "[%s] %s/%s: ", timebuf, level_str(level), tag ? tag : "lumi");

    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);

    fputc('\n', out);
}

const char *lumi_result_str(lumi_result_t code) {
    switch (code) {
        case LUMI_OK:             return "OK";
        case LUMI_ERR_NOMEM:      return "Out of memory";
        case LUMI_ERR_INVALID:    return "Invalid argument";
        case LUMI_ERR_NOT_FOUND:  return "Not found";
        case LUMI_ERR_IO:         return "I/O error";
        case LUMI_ERR_PERMISSION: return "Permission denied";
        case LUMI_ERR_TIMEOUT:    return "Timeout";
        default:                  return "Unknown error";
    }
}
