#pragma once

#include "canvas_object.h"

extern elem_object* new_marker_symbol(const char *name);
extern elem_object* new_marker_symbol(int n);
extern elem_object* new_marker_symbol_raw(const char *req_name, bool& stroke);
