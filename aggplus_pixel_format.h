#ifndef GRAPHICS_AGGPLUS_H
#define GRAPHICS_AGGPLUS_H

#include "agg_color_rgba.h"
#include "platform/agg_platform_support.h"

namespace aggplus {

struct pixel_format_rgb24 {
	typedef agg::order_rgb order;
	enum { size = 3, format = agg::pix_format_rgb24 };
};

}

#endif
