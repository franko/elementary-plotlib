#include "win32/window_win32.h"

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
    // m_bmp_draw(0),
    m_cur_x(0),
    m_cur_y(0),
    m_input_flags(0),
    m_redraw_flag(true),
    m_current_dc(0),
    m_width(0),
    m_height(0),
    m_is_mapped(false),
    m_is_ready(false),
    m_caption("Graphics Window"),
    m_target(tgt)
{
}

window_win32::~window_win32() {
#if 0
    if (m_bmp_draw) {
        delete [] (unsigned char*) m_bmp_draw;
    }
#endif
}

void window_win32::close() {
    ::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
}

// Attach the pixel_map image buffer to the rendering_buffer.
static inline void pixel_map_attach (pixel_map& pm, agg::rendering_buffer *rbuf, bool flip_y) {
    int stride = pm.stride();
    rbuf->attach(pm.buf(), pm.width(), pm.height(), flip_y ? stride : -stride);
}

#if 0
void window_win32::create_pmap(unsigned width, unsigned height, agg::rendering_buffer* wnd)
{
    m_pmap_window.create(width, height, org_e(graphics::bpp));
    pixel_map_attach(m_pmap_window, wnd, graphics::flip_y);
#if 0
    if (m_bmp_draw) {
        delete [] (unsigned char*) m_bmp_draw;
    }
    m_bmp_draw = pixel_map::create_bitmap_info(width, height, org_e(m_sys_bpp));
#endif
}
#endif

//------------------------------------------------------------------------
void window_win32::display_pmap(HDC dc, const agg::rendering_buffer* src, const agg::rect_base<int> *ri)
{
#if 0
    if(m_sys_format == graphics::pixel_format && ri == nullptr)
    {
        m_pmap_window.draw(dc);
    }
    else
    {
#endif
        agg::rect_base<int> r(0, 0, src->width(), src->height());
        if (ri) {
            r = agg::intersect_rectangles(r, *ri);
        }

        const int w = r.x2 - r.x1, h = r.y2 - r.y1;

        // In a previous version the bmp was stored in the class in m_bmp_draw and
        // resude by resizing to avoid allocating memory each time.
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

        unsigned int wh = m_height;
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
//    }
}

#if 0
bool window_win32::save_pmap(const char* fn, unsigned idx, const rendering_buffer* src) {
    pixel_map& img = m_pmap_img[idx];
    return img.save_as_bmp(fn);
}
#endif

#if 0
void window_win32::bitmap_info_resize (BITMAPINFO* bmp, unsigned w, unsigned h) {
    if (w == 0) w = 1;
    if (h == 0) h = 1;

    unsigned bits_per_pixel = bmp->bmiHeader.biBitCount;
    unsigned row_len = agg::pixel_map::calc_row_len(w, bits_per_pixel);

    bmp->bmiHeader.biWidth  = w;
    bmp->bmiHeader.biHeight = h;
    bmp->bmiHeader.biSizeImage = row_len * h;
}
#endif

void window_win32::get_module_instance() {
    g_windows_instance = GetModuleHandle(nullptr);
    g_windows_cmd_show = SW_SHOWNORMAL;
}

#if 0
void window_win32::resize(unsigned width, unsigned height) {
    m_target.resize(width, height);

    ?? delete m_draw_img;
    ?? m_draw_img = new(std::nothrow) x_image(m_sys_bpp, m_byte_order, width, height, &m_draw_conn);

    // m_width = width;
    // m_height = height;
}
#endif

LRESULT window_win32::proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC paintDC;

    HDC dc = ::GetDC(m_hwnd);
    m_current_dc = dc;
    LRESULT ret = 0;

    fprintf(stderr, "WINDOW PROC, msg: %d\n", msg); fflush(stderr);

    switch(msg) {
    case WM_CREATE:
        break;

    case WM_SIZE:
        m_width = LOWORD(lParam);
        m_height = HIWORD(lParam);
        // m_pmap_window.create(width, height, org_e(graphics::bpp));
        m_target.resize(m_width, m_height);
        // create_pmap(LOWORD(lParam), HIWORD(lParam), &app->rbuf_window());
        m_target.render();
        // app->trans_affine_resizing(LOWORD(lParam), HIWORD(lParam));
        // app->on_resize(LOWORD(lParam), HIWORD(lParam));
        // app->force_redraw();
        // draw_something(); /* FIXME */
        m_is_ready = false;
        break;

    case WM_ERASEBKGND:
        break;

    case WM_PAINT:
        paintDC = ::BeginPaint(hWnd, &ps);
        m_current_dc = paintDC;
        if (m_redraw_flag) {
            m_target.draw();
            m_redraw_flag = false;
        }
        // display_pmap(paintDC, &app->rbuf_window());
        // app->on_post_draw(paintDC);
        m_current_dc = 0;
        ::EndPaint(hWnd, &ps);

        m_is_mapped = true;
        m_is_ready = true;
        break;

    case WM_COMMAND:
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

    default:
        ret = ::DefWindowProc(hWnd, msg, wParam, lParam);
        break;
    }
    m_current_dc = 0;
    ::ReleaseDC(m_hwnd, dc);

    return ret;
}

LRESULT CALLBACK window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    void* user_data = reinterpret_cast<void*>(::GetWindowLongPtr(hWnd, GWL_USERDATA));
    window_win32* app = nullptr;

    // fprintf(stderr, "Hey there, window proc. Userdata: %p\n", user_data); fflush(stderr);

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

    fprintf(stderr, "Windows: init\n"); fflush(stderr);

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

    if(m_window_flags & window_resize)
    {
        wflags |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    m_hwnd = ::CreateWindow("AGGAppClass", m_caption.cstr(), wflags, 100, 100, width, height, 0, 0, g_windows_instance, 0);

    if(m_hwnd == 0) {
        return false;
    }

    m_redraw_flag = true;

    RECT rct;
    ::GetClientRect(m_hwnd, &rct);

    ::MoveWindow(m_hwnd,   // handle to window
                 100,      // horizontal position
                 100,      // vertical position
                 width + (width - (rct.right - rct.left)),
                 height + (height - (rct.bottom - rct.top)),
                 FALSE);

    // create_pmap(width, height, &m_rbuf_window);

    ::SetWindowLongPtr(m_hwnd, GWL_USERDATA, (LONG_PTR)this);
    ::ShowWindow(m_hwnd, g_windows_cmd_show);
    ::SetForegroundWindow(m_hwnd);
    return true;
}

void window_win32::update_region(graphics::image& src_img, const agg::rect_i& r) {
    fprintf(stderr, "NYI: called update region!\n");
}
