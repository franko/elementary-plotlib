
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "fatal.h"

void
fatal_exception(const char* msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    fputs("\n", stderr);
    va_end(ap);
    abort();
}
