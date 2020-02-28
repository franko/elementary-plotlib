#include <windows.h>

#include <agg_basics.h>
#include <agg_rendering_buffer.h>

#include "debug_priv.h"
#include "display_window_status.h"
#include "strpp.h"
#include "start_window.h"
#include "win32/agg_win32_bmp.h"
#include "window_close_callback.h"
#include "window_surface.h"

class window_win32 : public graphics::display_window_status {
public:
    window_win32(graphics::window_surface& window_surface);
    ~window_win32();

    void start_blocking(unsigned width, unsigned height, unsigned flags);

    void start(unsigned width, unsigned height, unsigned flags, window_close_callback *callback) {
        int status = start_window_new_thread(this, width, height, flags, callback);
        if (status != 0) {
            debug_log(1, "error starting window, unknwon error");
        }
    }

    bool send_request(graphics::window_request request_type, int plot_index) override;
    void update_region(const graphics::image& img, const agg::rect_i& r) override;

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
    graphics::window_surface& m_window_surface;
    int m_update_plot_index;

    static void get_module_instance();

    static HINSTANCE g_windows_instance;
    static int       g_windows_cmd_show;
};
