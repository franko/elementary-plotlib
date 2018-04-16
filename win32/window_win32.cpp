#include "win32/window_win32.h"
#include "debug_log.h"

//------------------------------------------------------------------------
HINSTANCE window_win32::g_windows_instance = 0;
int       window_win32::g_windows_cmd_show = 0;

#ifndef GWL_USERDATA
#define GWL_USERDATA (-21)
#endif

window_win32::window_win32(graphics::render_target& tgt) :
    m_sys_format(agg::pix_format_bgr24),
    m_sys_bpp(24),
    m_hwnd(0),
    m_cur_x(0),
    m_cur_y(0),
    m_input_flags(0),
    m_is_mapped(false),
    m_is_ready(false),
    m_caption("Graphics Window"),
    m_target(tgt)
{
}

window_win32::~window_win32() {
}

void window_win32::close() {
    ::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
}

// Attach the pixel_map image buffer to the rendering_buffer.
static inline void pixel_map_attach (pixel_map& pm, agg::rendering_buffer *rbuf, bool flip_y) {
    int stride = pm.stride();
    rbuf->attach(pm.buf(), pm.width(), pm.height(), flip_y ? stride : -stride);
}

void window_win32::display_pmap(HDC dc, const agg::rendering_buffer* src, const agg::rect_base<int> *ri) {
    agg::rect_base<int> r(0, 0, src->width(), src->height());
    if (ri) {
        r = agg::intersect_rectangles(r, *ri);
    }

    const int w = r.x2 - r.x1, h = r.y2 - r.y1;

    // In a previous version the bmp was stored in the class in m_bmp_draw and
    // was used by resizing to avoid allocating memory each time.
    BITMAPINFO *bmp = pixel_map::create_bitmap_info(w, h, org_e(m_sys_bpp));
    // bitmap_info_resize (m_bmp_draw, w, h);

    pixel_map pmap;
    pmap.attach_to_bmp(bmp);

    rendering_buffer rbuf_tmp;
    pixel_map_attach(pmap, &rbuf_tmp, graphics::flip_y);

    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, *src, r, graphics::bpp / 8);

    if (m_sys_format == graphics::pixel_format)
    {
        rbuf_tmp.copy_from(src_view);
    }
    else
    {
        if (m_sys_format == pix_format_bgr24 && graphics::pixel_format == pix_format_rgb24)
        {
            my_color_conv(&rbuf_tmp, &src_view, color_conv_rgb24_to_bgr24());
        }
    }

    unsigned int wh = src->height();
    RECT wrect;
    wrect.left   = r.x1;
    wrect.right  = r.x2;
    wrect.bottom = wh - r.y1;
    wrect.top    = wh - r.y2;

    RECT brect;
    brect.left   = 0;
    brect.right  = w;
    brect.bottom = h;
    brect.top    = 0;

    pmap.draw(dc, &wrect, &brect);

    delete [] (unsigned char*) bmp;
}

void window_win32::get_module_instance() {
    g_windows_instance = GetModuleHandle(nullptr);
    g_windows_cmd_show = SW_SHOWNORMAL;
}

LRESULT window_win32::proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    LRESULT ret = 0;

    switch(msg) {
    case WM_CREATE:
        debug_log("treating WM_CREATE event");
        break;

    case WM_SIZE: {
        const unsigned width = LOWORD(lParam), height = HIWORD(lParam);
        debug_log("treating WM_SIZE event width: %d height: %d", width, height);
        m_target.resize(width, height);
        m_target.render();
        m_is_ready = false;
        break;
    }

    case WM_ERASEBKGND:
        break;

    case WM_PAINT: {
        debug_log("treating WM_PAINT event");
        PAINTSTRUCT ps;
        HDC paintDC = ::BeginPaint(hWnd, &ps);
        m_target.draw();
        ::EndPaint(hWnd, &ps);
        m_is_mapped = true;
        m_is_ready = true;
        break;
    }

    case WM_COMMAND:
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

    default:
        ret = ::DefWindowProc(hWnd, msg, wParam, lParam);
        break;
    }

    return ret;
}

LRESULT CALLBACK window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    void* user_data = reinterpret_cast<void*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));
    window_win32* app = nullptr;

    if (user_data) {
        app = reinterpret_cast<window_win32 *>(user_data);
    }

    if(app == nullptr) {
        if(msg == WM_DESTROY) {
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return app->proc(hWnd, msg, wParam, lParam);
}


bool window_win32::init(unsigned width, unsigned height, unsigned flags)
{
    if (g_windows_instance == 0) {
        get_module_instance();
    }

    m_window_flags = flags;

    int wflags = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

    WNDCLASS wc;
    wc.lpszClassName = "AGGAppClass";
    wc.lpfnWndProc = window_proc;
    wc.style = wflags;
    wc.hInstance = g_windows_instance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName = "AGGAppMenu";
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    ::RegisterClass(&wc);

    wflags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    if(m_window_flags & window_resize) {
        wflags |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    m_hwnd = ::CreateWindow("AGGAppClass", m_caption.cstr(), wflags, 100, 100, width, height, 0, 0, g_windows_instance, 0);

    if(m_hwnd == 0) {
        return false;
    }

    RECT rct;
    ::GetClientRect(m_hwnd, &rct);

    ::MoveWindow(m_hwnd,   // handle to window
                 100,      // horizontal position
                 100,      // vertical position
                 width + (width - (rct.right - rct.left)),
                 height + (height - (rct.bottom - rct.top)),
                 FALSE);

    ::SetWindowLongPtr(m_hwnd, GWL_USERDATA, (LONG_PTR)this);
    ::ShowWindow(m_hwnd, g_windows_cmd_show);
    ::SetForegroundWindow(m_hwnd);
    return true;
}

int window_win32::run() {
    MSG msg;

    for(;;) {
        bool status;

        if (m_is_ready) {
            m_mutex.unlock();
            status = ::GetMessage(&msg, 0, 0, 0);
            m_mutex.lock();
        } else {
            status = ::GetMessage(&msg, 0, 0, 0);
        }

        if (!status) {
            break;
        }
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void window_win32::update_region(graphics::image& src_img, const agg::rect_i& r) {
    debug_log("update_region: %d %d %d %d", r.x1, r.y1, r.x2, r.y2);
    HDC dc = ::GetDC(m_hwnd);
    display_pmap(dc, &src_img, &r);
    ::ReleaseDC(m_hwnd, dc);
}
