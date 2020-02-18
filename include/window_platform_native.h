#pragma once

#include "window_gen.h"

#if defined (WIN32) || defined (CYGWIN)
#include "win32/window_win32.h"
typedef window_gen<window_win32> elem_window_native;
#else
#include "xwindow/xwindow.h"
typedef window_gen<xwindow> elem_window_native;
#endif
