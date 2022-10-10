#include "global_elem_window_factory.h"

#include "sdl_elem_window_factory.h"
elem_window_factory *global_elem_window_factory = new sdl_elem_window_factory{};
