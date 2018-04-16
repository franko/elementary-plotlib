#ifndef LIBCANVAS_DEBUG_LOG_H
#define LIBCANVAS_DEBUG_LOG_H

#ifdef LIBCANVAS_DEBUG
#include <cstdio>
#include <cstdarg>

static void debug_log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fputs("\n", stderr);
    fflush(stderr);
    va_end(args);
}

#else

static void debug_log(const char *fmt, ...) {
}

#endif

#endif
