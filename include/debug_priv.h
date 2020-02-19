#pragma once

#ifdef ELEM_PLOT_DEBUG
#include <cstdio>
#include <cstdarg>
#endif

#ifndef ELEM_PLOT_DEBUG_LEVEL
#define ELEM_PLOT_DEBUG_LEVEL 1
#endif

#ifdef ELEM_PLOT_DEBUG
inline void debug_log(int level, const char *fmt, ...) {
    if (level <= ELEM_PLOT_DEBUG_LEVEL) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        fputs("\n", stderr);
        fflush(stderr);
        va_end(args);
    }
}
#else
inline void debug_log(int level, const char *fmt, ...) {
}
#endif
