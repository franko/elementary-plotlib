#include "global_elem_window_factory.h"
#include "native_elem_window_factory.h"
#include "sdl_elem_window_factory.h"

#ifdef ELEM_PLOT_USE_SDL
elem_window_factory *global_elem_window_factory = new sdl_elem_window_factory{};
#else
elem_window_factory *global_elem_window_factory = new sdl_elem_window_factory{};
#endif
