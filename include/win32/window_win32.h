#include <windows.h>
#include <agg_basics.h>
#include <agg_rendering_buffer.h>

#include "win32/agg_win32_bmp.h"
#include "strpp.h"
#include "render_config.h"
#include "pthreadpp.h"
#include "window_surface.h"

class window_win32 {
public:
    window_win32(graphics::render_target& tgt);
    ~window_win32();

    bool init(unsigned width, unsigned height, unsigned flags);
    void run();
    void close();

    void update_region(graphics::image& src_img, const agg::rect_i& r);

    void lock()   { m_mutex.lock();   }
    void unlock() { m_mutex.unlock(); }

private:
    void create_pmap(unsigned width, unsigned height, agg::rendering_buffer* wnd);
    void display_pmap(HDC dc, const agg::rendering_buffer* src, const agg::rect_base<int> *rect = 0);
    bool save_pmap(const char* fn, unsigned idx, const rendering_buffer* src);

    void resize(unsigned width, unsigned height);

    LRESULT window_win32::proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void get_module_instance();

    // pix_format_e  m_format;
    pix_format_e  m_sys_format;
    // bool          m_flip_y;
    // unsigned      m_bpp;
    unsigned      m_sys_bpp;
    HWND          m_hwnd;
    pixel_map     m_pmap_window;
    BITMAPINFO*   m_bmp_draw;
    pixel_map     m_pmap_img[platform_support::max_images];
    int           m_cur_x;
    int           m_cur_y;
    unsigned      m_input_flags;
    bool          m_redraw_flag;
    HDC           m_current_dc;

    bool m_is_mapped;
    bool m_is_ready;

    str m_caption;
    pthread::mutex m_mutex;
    graphics::render_target& m_target;

    static void bitmap_info_resize(BITMAPINFO* bmp, unsigned w, unsigned h);

    static HINSTANCE g_windows_instance;
    static int       g_windows_cmd_show;
};
