#include <windows.h>
#include <agg_basics.h>
#include <agg_rendering_buffer.h>

#include "win32/agg_win32_bmp.h"
#include "strpp.h"
#include "render_config.h"
#include "pthreadpp.h"
#include "window_surface.h"
#include "window_flags.h"

class window_win32 : public graphics::display_window {
    enum { window_not_started, window_starting, window_ready, window_closed };

public:
    window_win32(graphics::render_target& tgt);
    ~window_win32();

    bool init(unsigned width, unsigned height, unsigned flags);
    int run();
    void close();

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);

    virtual void lock()   { m_mutex.lock();   }
    virtual void unlock() { m_mutex.unlock(); }

    LRESULT proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    void display_pmap(HDC dc, const agg::rendering_buffer* src, const agg::rect_base<int> *rect = 0);

    pix_format_e  m_sys_format;
    unsigned      m_sys_bpp;
    HWND          m_hwnd;

    int m_window_status;
    str m_caption;
    pthread::mutex m_mutex;
    graphics::render_target& m_target;

    static void get_module_instance();

    static HINSTANCE g_windows_instance;
    static int       g_windows_cmd_show;
};
