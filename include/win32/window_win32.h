#include <windows.h>

#include <agg_basics.h>
#include <agg_rendering_buffer.h>

#include "debug_priv.h"
#include "display_window_status.h"
#include "strpp.h"
#include "update_region_info.h"
#include "start_window.h"
#include "win32/agg_win32_bmp.h"
#include "window_surface.h"

class window_win32 : public graphics::display_window_status {
public:
    window_win32(graphics::window_surface& window_surface);
    ~window_win32();

    void start_blocking(unsigned width, unsigned height, unsigned flags);

    void start(unsigned width, unsigned height, unsigned flags) {
        int status = start_window_new_thread(this, width, height, flags);
        if (status != 0) {
            debug_log(1, "error starting window, unknwon error");
        }
    }

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);
    virtual void update_region_request(graphics::image& img, const agg::rect_i& r);

    LRESULT proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    enum { WM_ELEM_UPD_REGION = WM_USER };

    bool init(unsigned width, unsigned height, unsigned flags);
    int run();
    void close();

    void display_pmap(HDC dc, const agg::rendering_buffer* src, const agg::rect_base<int> *rect = 0);

    pix_format_e  m_sys_format;
    unsigned      m_sys_bpp;
    HWND          m_hwnd;

    str m_caption;
    update_region_info m_update_region;
    graphics::window_surface& m_window_surface;

    static void get_module_instance();

    static HINSTANCE g_windows_instance;
    static int       g_windows_cmd_show;
};
