#include <thread>
#include <mutex>
#include <condition_variable>

#include <windows.h>

#include <agg_basics.h>
#include <agg_rendering_buffer.h>

#include "win32/agg_win32_bmp.h"
#include "strpp.h"
#include "render_config.h"
#include "window_surface.h"
#include "window_flags.h"

class window_win32 : public graphics::display_window {
public:
    window_win32(graphics::render_target& tgt);
    ~window_win32();

    bool init(unsigned width, unsigned height, unsigned flags);
    int run();
    void close();
    void wait_running(std::unique_lock<std::mutex>& lock);
    std::unique_lock<std::mutex> get_lock() { return std::unique_lock<std::mutex>(m_mutex); }

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);

    virtual void lock()   { m_mutex.lock();   }
    virtual void unlock() { m_mutex.unlock(); }
    virtual int status() { return m_window_status; }

    LRESULT proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    static void run_window(window_win32 *window, unsigned width, unsigned height, unsigned flags);

    void display_pmap(HDC dc, const agg::rendering_buffer* src, const agg::rect_base<int> *rect = 0);
    void send_ready_message();

    pix_format_e  m_sys_format;
    unsigned      m_sys_bpp;
    HWND          m_hwnd;

    int m_window_status;
    str m_caption;
    std::mutex m_mutex;
    std::condition_variable m_running; // Used to signal when window is running.
    graphics::render_target& m_target;

    static void get_module_instance();

    static HINSTANCE g_windows_instance;
    static int       g_windows_cmd_show;
};
