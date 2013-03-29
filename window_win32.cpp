#include "fatal.h"

static HINSTANCE g_windows_instance = 0;
static int g_windows_cmd_show = 0;

// we use the bgr24 pixel format as the native pixel
enum { sys_bpp = 24, sys_pixel_size = 3, sys_pixel_format = agg::pix_format_bgr24 };

static void bitmap_info_resize (BITMAPINFO* bmp, unsigned w, unsigned h)
{
    if (w == 0) w = 1;
    if (h == 0) h = 1;

    unsigned bits_per_pixel = bmp->bmiHeader.biBitCount;
    unsigned row_len = agg::pixel_map::calc_row_len(w, bits_per_pixel);

    bmp->bmiHeader.biWidth  = w;
    bmp->bmiHeader.biHeight = h;
    bmp->bmiHeader.biSizeImage = row_len * h;
}
/*
static void pixel_map_attach (agg::pixel_map& pm, agg::rendering_buffer *rbuf, bool flip_y)
{
    int stride = pm.stride();
    rbuf->attach(pm.buf(), pm.width(), pm.height(), flip_y ? stride : -stride);
}
*/
//------------------------------------------------------------------------
void window_win32::create_pmap(unsigned width, unsigned height, rendering_buffer* rbuf)
{
	const unsigned bpp = graphics::pixel_size * 8;
    m_pmap_window.create(width, height, org_e(bpp));
    pixel_map_attach(m_pmap_window, rbuf, graphics::flip_y);
    delete [] (unsigned char*) m_bmp_draw;
    m_bmp_draw = pixel_map::create_bitmap_info(width, height, org_e(sys_bpp));
}

//------------------------------------------------------------------------
void window_win32::display_pmap(HDC dc, const rendering_buffer* src, const agg::rect_i* ri)
{
    if(sys_pixel_format == graphics::pixel_format && ri == 0)
    {
        m_pmap_window.draw(dc);
    }
    else
    {
        agg::rect_i r(0, 0, src->width(), src->height());
        if (ri) {
            r = agg::intersect_rectangles(r, *ri);
		}
        int w = r.x2 - r.x1, h = r.y2 - r.y1;

        bitmap_info_resize(m_bmp_draw, w, h);

        pixel_map pmap;
        pmap.attach_to_bmp(m_bmp_draw);

        rendering_buffer rbuf_tmp;
        pixel_map_attach (pmap, &rbuf_tmp, m_flip_y);

        rendering_buffer_ro src_view;
        rendering_buffer_get_const_view(src_view, *src, r, graphics::bpp / 8);

        if (sys_pixel_format == graphics::pixel_format) {
            rbuf_tmp.copy_from(src_view);
        } else {
            if (sys_pixel_format == agg::pix_format_bgr24 && graphics::format == agg::pix_format_rgb24) {
                my_color_conv(&rbuf_tmp, &src_view, color_conv_rgb24_to_bgr24());
            } else {
            	fatal_exception("unsupported pixel format, system: %d, application: %d", int(sys_pixel_format), int(graphics::pixel_format));
            }
        }

        unsigned int wh = m_pmap_window.height();
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
    }
}

void window_win32::resize(unsigned width, unsigned height)
{
    m_target.resize(width, height);
    create_pmap(LOWORD(lParam), HIWORD(lParam), &app->rbuf_window());
    m_draw_img = new(std::nothrow) x_image(m_sys_bpp, m_byte_order, width, height, &m_draw_conn);

    m_width = width;
    m_height = height;
}

LRESULT CALLBACK window_win32::wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC paintDC;

    void* user_data = reinterpret_cast<void*>(::GetWindowLong(hWnd, GWL_USERDATA));
    window_win32* win = (user_data ? reinterpret_cast<window_win32*>(user_data) : 0);

    if(win == 0) {
        if(msg == WM_DESTROY) {
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    HDC dc = ::GetDC(win->m_hwnd);
    win->m_current_dc = dc;
    LRESULT ret = 0;

    switch(msg)
    {
        //--------------------------------------------------------------------
    case WM_CREATE:
        break;

        //--------------------------------------------------------------------
    case WM_SIZE:
        win->create_pmap(LOWORD(lParam), HIWORD(lParam), &app->rbuf_window());
        app->trans_affine_resizing(LOWORD(lParam), HIWORD(lParam));
        app->on_resize(LOWORD(lParam), HIWORD(lParam));
        app->force_redraw();
        app->m_specific->m_is_ready = false;

        break;

        //--------------------------------------------------------------------
    case WM_ERASEBKGND:
        break;

        //--------------------------------------------------------------------
    case WM_PAINT:
        paintDC = ::BeginPaint(hWnd, &ps);
        app->m_specific->m_current_dc = paintDC;
        if(app->m_specific->m_redraw_flag)
        {
            app->on_draw();
            app->m_specific->m_redraw_flag = false;
        }
        app->m_specific->display_pmap(paintDC, &app->rbuf_window());
        app->on_post_draw(paintDC);
        app->m_specific->m_current_dc = 0;
        ::EndPaint(hWnd, &ps);

        app->m_specific->m_is_mapped = true;
        app->m_specific->m_is_ready = true;
        break;

        //--------------------------------------------------------------------
    case WM_COMMAND:
        break;

        //--------------------------------------------------------------------
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;

        //--------------------------------------------------------------------
    default:
        ret = ::DefWindowProc(hWnd, msg, wParam, lParam);
        break;
    }
    app->m_specific->m_current_dc = 0;
    ::ReleaseDC(app->m_specific->m_hwnd, dc);

    return ret;
}

bool window_win32::init(unsigned width, unsigned height, unsigned flags)
{
    m_window_flags = flags;

    int wflags = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

    WNDCLASS wc;
    wc.lpszClassName = "PlotAppClass";
    wc.lpfnWndProc = (WNDPROC) wnd_proc;
    wc.style = wflags;
    wc.hInstance = g_windows_instance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.lpszMenuName = "PlotppMenu";
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    ::RegisterClass(&wc);

    wflags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    if(m_window_flags & window_resize)
    {
        wflags |= WS_THICKFRAME | WS_MAXIMIZEBOX;
    }

    const char* caption = m_caption.cstr();
    m_hwnd = ::CreateWindow("PlotAppClass", caption, wflags, 100, 100, width, height, 0, 0, g_windows_instance, 0);

    if(m_hwnd == 0) {
        return false;
    }

    RECT rct;
    ::GetClientRect(m_hwnd, &rct);

    ::MoveWindow(m_hwnd,               // handle to window
                 100,                  // horizontal position
                 100,                  // vertical position
                 width + (width - (rct.right - rct.left)),
                 height + (height - (rct.bottom - rct.top)),
                 FALSE);

    ::SetWindowLong(m_hwnd, GWL_USERDATA, (LONG)this);

    m_specific->create_pmap(width, height, &m_rbuf_window);
    // resize(width, height);
    m_target.render();

#if 0
    m_initial_width = width;
    m_initial_height = height;
    on_init();
    m_specific->m_redraw_flag = true;
#endif

    ::ShowWindow(m_hwnd, g_windows_cmd_show);
    ::SetForegroundWindow(m_hwnd);
    return true;
}

void
window_win32::prepare()
{
    if (g_windows_instance == 0)
    {
        g_windows_instance = GetModuleHandle(NULL);
        g_windows_cmd_show = SW_SHOWNORMAL;
    }
}
