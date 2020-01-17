#pragma once

#include "window_gen.h"

#if defined (WIN32) || defined (CYGWIN)
#include "win32/window_win32.h"
typedef window_gen<window_win32> elp_window_native;
#else
#include "xwindow/xwindow.h"
typedef window_gen<xwindow> elp_window_native;
#endif
