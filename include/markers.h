#pragma once

#include "canvas_object.h"

extern elp_object* new_marker_symbol(const char *name);
extern elp_object* new_marker_symbol(int n);
extern elp_object* new_marker_symbol_raw(const char *req_name, bool& stroke);
