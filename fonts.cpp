
#include "render_config.h"
#include "fatal.h"

agg::font_engine_freetype_int32 global_font_eng;
agg::font_cache_manager<agg::font_engine_freetype_int32> global_font_man(global_font_eng);

int initialize_fonts()
{
    const char* font_name = gslshell::get_font_name();
    if (!font_name)
        fatal_exception("cannot find a suitable truetype font");
    agg::glyph_rendering gren = agg::glyph_ren_outline;
    if (!global_font_eng.load_font(font_name, 0, gren))
        fatal_exception("cannot load truetype font");
    global_font_eng.hinting(true);
    return 0;
}

agg::font_engine_freetype_int32& gslshell::font_engine()
{
    return global_font_eng;
}

agg::font_cache_manager<agg::font_engine_freetype_int32>& gslshell::font_manager()
{
    return global_font_man;
}
