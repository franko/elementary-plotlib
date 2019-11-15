#if defined (WIN32) || defined (CYGWIN)
#include <windows.h>
#include <shlobj.h>
#endif
#include <sys/stat.h>

#include "render_config.h"
#include "fatal.h"

#ifndef DISABLE_GAMMA_CORR
graphics::gamma_type graphics::gamma(1.5);
#endif
// Original values for the subpixel color filter. These are quite conservative
// to avoid color fringes.
// agg::lcd_distribution_lut graphics::subpixel_lut(3./9., 2./9., 1./9.);

// Slightly more agressive values with more weight to primary channel.
// Some people may notice colorer fringes but fonts looks sharper.
agg::lcd_distribution_lut graphics::subpixel_lut(0.448, 0.184, 0.092);

agg::font_engine_freetype_int32 global_font_eng;
agg::font_cache_manager<agg::font_engine_freetype_int32> global_font_man(global_font_eng);

#if defined (WIN32) || defined (CYGWIN)
const char *ttf_names[] = {"calibri.ttf", "arial.ttf", 0};
const char *console_font_names[] = {"consolas", "lucida console", "fixedsys", 0};
const char *console_font_filenames[] = {"consola.ttf", "lucon.ttf", "cvgafix.fon", 0};
#elif defined (DARWIN_MACOSX)
const char *ttf_names[] = {"Arial.ttf", "Trebuchet MS.ttf", "Courier New.ttf", 0};
#define TTF_SYSTEM_DIR "/Library/Fonts/"
#define CONSOLE_FONT_NAME "Monaco"
#else
const char *ttf_names[] = {"ubuntu-font-family/Ubuntu-R.ttf", "freefont/FreeSans.ttf", "ttf-dejavu/DejaVuSans.ttf", 0};
#define TTF_SYSTEM_DIR "/usr/share/fonts/truetype/"
#define CONSOLE_FONT_NAME "monospace"
#endif

#if defined (WIN32) || defined (CYGWIN)

#ifdef CYGWIN
#define MYWSTRUCT_STAT struct stat
#define MYWSTAT stat
#else
#define MYWSTRUCT_STAT struct _stat
#define MYWSTAT _stat
#endif

const char *graphics::get_font_name()
{
    static TCHAR pf[MAX_PATH + 32];
    SHGetFolderPath(0, CSIDL_FONTS, NULL, 0, pf);
    unsigned len = strlen(pf);
    pf[len++] = '\\';
    for (int k = 0; ttf_names[k]; k++)
    {
        const char* font_name = ttf_names[k];
        memcpy(pf + len, font_name, (strlen(font_name) + 1) * sizeof(char));
        MYWSTRUCT_STAT inf[1];
        int status = MYWSTAT(pf, inf);
        if (status == 0)
            return pf;
    }

    return 0;
}

const char* graphics::get_fox_console_font_name()
{
    static TCHAR pf[MAX_PATH + 32];
    SHGetFolderPath(0, CSIDL_FONTS, NULL, 0, pf);
    unsigned len = strlen(pf);
    pf[len++] = '\\';
    for (int k = 0; console_font_filenames[k]; k++)
    {
        const char* filename = console_font_filenames[k];
        memcpy(pf + len, filename, (strlen(filename) + 1) * sizeof(char));
        MYWSTRUCT_STAT inf[1];
        int status = MYWSTAT(pf, inf);
        if (status == 0)
            return console_font_names[k];
    }

    return 0;
}

#else

const char *graphics::get_font_name()
{
    const char* ttf_dir = TTF_SYSTEM_DIR;
    unsigned len = strlen(ttf_dir);
    static char pf[256];
    memcpy(pf, ttf_dir, len + 1);
    for (int k = 0; ttf_names[k]; k++)
    {
        const char* font_name = ttf_names[k];
        memcpy(pf + len, font_name, (strlen(font_name) + 1) * sizeof(char));
        struct stat inf[1];
        int status = stat(pf, inf);
        if (status == 0)
            return pf;
    }

    return 0;
}

const char* graphics::get_fox_console_font_name()
{
    return CONSOLE_FONT_NAME;
}

#endif

int graphics::initialize_fonts()
{
    const char* font_name = graphics::get_font_name();
    if (!font_name)
        fatal_exception("cannot find a suitable truetype font");
    agg::glyph_rendering gren = agg::glyph_ren_outline;
    if (!global_font_eng.load_font(font_name, 0, gren))
        fatal_exception("cannot load truetype font");
    global_font_eng.hinting(true);
    return 0;
}

agg::font_engine_freetype_int32& graphics::font_engine()
{
    return global_font_eng;
}

agg::font_cache_manager<agg::font_engine_freetype_int32>& graphics::font_manager()
{
    return global_font_man;
}
