#pragma once

#ifdef WIN32
#include <synchapi.h>
// Include only syncapi.h to have the defition of Sleep function
// without all the windows.h stuff.
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
