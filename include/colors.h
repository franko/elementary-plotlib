#pragma once

#include "defs.h"
#include "agg_color_rgba.h"

namespace colors {

inline static agg::rgba8 black() {
    return agg::rgba8(0,0,0);
}

inline static agg::rgba8 white() {
    return agg::rgba8(255,255,255);
}

inline static agg::rgba8 zero() {
    return agg::rgba8(0,0,0,0);
}

};
