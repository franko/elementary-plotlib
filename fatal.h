#ifndef MY_FATAL_H
#define MY_FATAL_H

#include "defs.h"

extern void fatal_exception(const char* msg) __attribute__ ((noreturn));

#endif
