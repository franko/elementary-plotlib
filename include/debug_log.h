#ifndef LIBCANVAS_DEBUG_LOG_H
#define LIBCANVAS_DEBUG_LOG_H

#ifdef WIN32
#include <windows.h>
inline void sleep(int s) { Sleep(s * 1000); }
#else
#include <unistd.h>
#endif

#ifdef LIBCANVAS_DEBUG
#include <cstdio>
#include <cstdarg>

inline void debug_log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fputs("\n", stderr);
    fflush(stderr);
    va_end(args);
}

#else

inline void debug_log(const char *fmt, ...) {
}

#endif

#endif
