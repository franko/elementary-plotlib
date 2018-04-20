#ifndef RENDER_CONFIG_H
#define RENDER_CONFIG_H

#include <mutex>

#include "agg_gamma_lut.h"
#include "agg_pixfmt_rgb24_lcd.h"
#include "agg_font_freetype.h"
#include "platform/agg_platform_support.h"

namespace graphics
{
typedef agg::gamma_lut<agg::int8u, agg::int16u, 8, 12> gamma_type;

static constexpr agg::pix_format_e pixel_format = agg::pix_format_rgb24;
static constexpr int pixel_size = 3, bpp = 24;
static constexpr bool flip_y = true;

extern gamma_type gamma;
extern agg::lcd_distribution_lut subpixel_lut;
extern std::mutex drawing_mutex;

extern agg::font_engine_freetype_int32& font_engine();
extern agg::font_cache_manager<agg::font_engine_freetype_int32>& font_manager();

extern const char *get_font_name();
extern const char *get_fox_console_font_name();

extern int initialize_fonts();
}

#endif
