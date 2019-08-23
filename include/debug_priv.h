#pragma once

#ifdef LIBCANVAS_DEBUG
#include <cstdio>
#include <cstdarg>
#endif

#ifndef LIBCANVAS_DEBUG_LEVEL
#define LIBCANVAS_DEBUG_LEVEL 1
#endif

#ifdef LIBCANVAS_DEBUG
inline void debug_log(int level, const char *fmt, ...) {
    if (level >= LIBCANVAS_DEBUG_LEVEL) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        fputs("\n", stderr);
        fflush(stderr);
        va_end(args);
    }
}
#else
inline void debug_log(const char *fmt, ...) {
}
#endif
