#include "global_elem_window_factory.h"

#ifdef ELEM_PLOT_USE_SDL
#include "sdl_elem_window_factory.h"
elem_window_factory *global_elem_window_factory = new sdl_elem_window_factory{};
#else
#include "native_elem_window_factory.h"
elem_window_factory *global_elem_window_factory = new native_elem_window_factory{};
#endif
