#pragma once

#include "window_gen.h"

#if defined (WIN32) || defined (CYGWIN)
#include "win32/window_win32.h"
typedef window_gen<window_win32> elem_window_native;
#elif __linux__
#include "xwindow/xwindow.h"
typedef window_gen<xwindow> elem_window_native;
#else
#include "sdl/window_sdl.h"
typedef window_gen<window_sdl> elem_window_native;
#endif
