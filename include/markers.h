#pragma once

#include "sg_object.h"

extern sg_object* new_marker_symbol(const char *name);
extern sg_object* new_marker_symbol(int n);
extern sg_object* new_marker_symbol_raw(const char *req_name, bool& stroke);
