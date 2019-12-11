#pragma once

#ifdef LIBELPLOT_DEBUG
#include <cstdio>
#include <cstdarg>
#endif

#ifndef LIBELPLOT_DEBUG_LEVEL
#define LIBELPLOT_DEBUG_LEVEL 1
#endif

#ifdef LIBELPLOT_DEBUG
inline void debug_log(int level, const char *fmt, ...) {
    if (level >= LIBELPLOT_DEBUG_LEVEL) {
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
