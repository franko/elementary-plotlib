#ifndef RENDER_CONFIG_H
#define RENDER_CONFIG_H

#include "aggplus_pixel_format.h"
#include "agg_gamma_lut.h"
#include "agg_pixfmt_rgb24_lcd.h"
#include "agg_font_freetype.h"

namespace graphics
{
typedef agg::gamma_lut<agg::int8u, agg::int16u, 8, 12> gamma_type;
typedef aggplus::pixel_format_rgb24 pixel_format;

const bool flip_y = true;

extern unsigned      bpp;
extern agg::pix_format_e  sys_pixel_format;
extern unsigned      sys_bpp;

extern gamma_type gamma;
extern agg::lcd_distribution_lut subpixel_lut;

extern agg::font_engine_freetype_int32& font_engine();
extern agg::font_cache_manager<agg::font_engine_freetype_int32>& font_manager();

extern const char *get_font_name();
extern const char *get_fox_console_font_name();

extern int initialize_fonts();
}

#endif
