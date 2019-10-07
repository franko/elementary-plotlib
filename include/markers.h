#pragma once

#include "canvas_object.h"

extern canvas_object* new_marker_symbol(const char *name);
extern canvas_object* new_marker_symbol(int n);
extern canvas_object* new_marker_symbol_raw(const char *req_name, bool& stroke);
