#include <X.h>
#include <Xutil.h>

#include "xwindow.h"
#include "fatal.h"

xwindow::xwindow(render_target& tgt):
    m_sys_format(agg::pix_format_undefined),
    m_byte_order(LSBFirst),
    m_sys_bpp(0),
    m_width(0), m_height(0),
    m_window(0),
    m_gc(0),
    m_close_atom(0),
    m_wm_protocols_atom(0),
    m_target(tgt)
{
    const agg::pix_format_e nat_format = (agg::pix_format_e) graphics::pixel_format;

    switch(nat_format)
    {
    default:
        break;
    case agg::pix_format_gray8:
        m_bpp = 8;
        break;

    case agg::pix_format_rgb565:
    case agg::pix_format_rgb555:
        m_bpp = 16;
        break;

    case agg::pix_format_rgb24:
    case agg::pix_format_bgr24:
        m_bpp = 24;
        break;

    case agg::pix_format_bgra32:
    case agg::pix_format_abgr32:
    case agg::pix_format_argb32:
    case agg::pix_format_rgba32:
        m_bpp = 32;
        break;
    }
}

void xwindow::close_connections()
{
    m_draw_conn.close();
    m_main_conn.close();
}

void xwindow::free_x_resources()
{
    delete m_draw_img;
    XFreeGC(m_main_conn.display, m_gc);
}

void xwindow::caption(const str& text)
{
    // Fixed by Enno Fennema (in original AGG library)
    const char *capt = text.cstr();
    Display *d = m_main_conn.display;
    XStoreName(d, m_window, capt);
    XSetIconName(d, m_window, capt);
}

bool xwindow::init(unsigned width, unsigned height, unsigned flags)
{
    m_window_flags = flags;

    if (unlikely(!m_main_conn.init()))
        return false;

    if (unlikely(!m_draw_conn.init()))
    {
        close_connections();
        return false;
    }

    x_connection *xc = &this->m_main_conn;

    unsigned long r_mask = xc->visual->red_mask;
    unsigned long g_mask = xc->visual->green_mask;
    unsigned long b_mask = xc->visual->blue_mask;

    if (xc->depth < 15 || r_mask == 0 || g_mask == 0 || b_mask == 0)
    {
        close_connections();
        fatal_exception(
            "There's no Visual compatible with minimal AGG requirements:\n"
            "At least 15-bit color depth and True- or DirectColor class.\n\n");
    }

    int t = 1;
    int hw_byte_order = LSBFirst;
    if(*(char*)&t == 0) hw_byte_order = MSBFirst;

    // Perceive SYS-format by mask
    switch(xc->depth)
    {
    case 15:
        m_sys_bpp = 16;
        if(r_mask == 0x7C00 && g_mask == 0x3E0 && b_mask == 0x1F)
        {
            m_sys_format = agg::pix_format_rgb555;
            m_byte_order = hw_byte_order;
        }
        break;

    case 16:
        m_sys_bpp = 16;
        if(r_mask == 0xF800 && g_mask == 0x7E0 && b_mask == 0x1F)
        {
            m_sys_format = agg::pix_format_rgb565;
            m_byte_order = hw_byte_order;
        }
        break;

    case 24:
    case 32:
        m_sys_bpp = 32;
        if(g_mask == 0xFF00)
        {
            if(r_mask == 0xFF && b_mask == 0xFF0000)
            {
                switch(graphics::pixel_format)
                {
                case agg::pix_format_rgba32:
                    m_sys_format = agg::pix_format_rgba32;
                    m_byte_order = LSBFirst;
                    break;

                case agg::pix_format_abgr32:
                    m_sys_format = agg::pix_format_abgr32;
                    m_byte_order = MSBFirst;
                    break;

                default:
                    m_byte_order = hw_byte_order;
                    m_sys_format =
                        (hw_byte_order == LSBFirst) ?
                        agg::pix_format_rgba32 :
                        agg::pix_format_abgr32;
                    break;
                }
            }

            if(r_mask == 0xFF0000 && b_mask == 0xFF)
            {
                switch(graphics::pixel_format)
                {
                case agg::pix_format_argb32:
                    m_sys_format = agg::pix_format_argb32;
                    m_byte_order = MSBFirst;
                    break;

                case agg::pix_format_bgra32:
                    m_sys_format = agg::pix_format_bgra32;
                    m_byte_order = LSBFirst;
                    break;

                default:
                    m_byte_order = hw_byte_order;
                    m_sys_format =
                        (hw_byte_order == MSBFirst) ?
                        agg::pix_format_argb32 :
                        agg::pix_format_bgra32;
                    break;
                }
            }
        }
        break;
    }

    if(m_sys_format == agg::pix_format_undefined)
    {
        close_connections();
        fatal_exception(
            "RGB masks are not compatible with AGG pixel formats:\n"
            "R=%08x, R=%08x, B=%08x\n",
            (unsigned)r_mask, (unsigned)g_mask, (unsigned)b_mask);
    }

    XSetWindowAttributes *win_attr = &m_window_attributes;
    memset(win_attr, 0, sizeof(XSetWindowAttributes));
    win_attr->override_redirect = 0;

    unsigned long window_mask = 0;

    m_window =
        XCreateWindow(xc->display, XDefaultRootWindow(xc->display),
                      0, 0, width, height,
                      0, xc->depth, InputOutput, CopyFromParent,
                      window_mask, win_attr);

    m_gc = XCreateGC(xc->display, m_window, 0, 0);

    caption(m_caption);

    XSizeHints *hints = XAllocSizeHints();
    if(hints)
    {
        if(flags & window_resize)
        {
            hints->min_width  = window_min_width;
            hints->min_height = window_min_height;
            hints->max_width  = window_max_width;
            hints->max_height = window_max_height;
        }
        else
        {
            hints->min_width  = width;
            hints->min_height = height;
            hints->max_width  = width;
            hints->max_height = height;
        }
        hints->flags = PMaxSize | PMinSize;

        XSetWMNormalHints(xc->display, m_window, hints);
        XFree(hints);
    }

    XMapWindow(xc->display, m_window);
    XSelectInput(xc->display, m_window, xevent_mask);

    m_close_atom = XInternAtom(xc->display, "WM_DELETE_WINDOW", false);
    m_wm_protocols_atom = XInternAtom(xc->display, "WM_PROTOCOLS", true);
    XSetWMProtocols(xc->display, m_window, &m_close_atom, 1);

    wait_map_notify();
    resize(width, height);
    m_target.render();

    return true;
}

void xwindow::wait_map_notify()
{
    x_connection *xc = &this->m_main_conn;
    XFlush(xc->display);
    XEvent ev;
    do
    {
        XNextEvent(xc->display, &ev);
    }
    while (ev.type != MapNotify);
}

void xwindow::resize(unsigned width, unsigned height)
{
    m_target.resize(width, height);
    m_draw_img = new(std::nothrow) x_image(m_sys_bpp, m_byte_order, width, height, &m_draw_conn);

    m_width = width;
    m_height = height;
}

void xwindow::run()
{
    x_connection *xc = &this->m_main_conn;

    bool quit = false;

    while (!quit)
    {
        XEvent ev;

        XNextEvent(xc->display, &ev);

        m_mutex.lock();
        switch(ev.type)
        {
        case ConfigureNotify:
        {
            unsigned width  = ev.xconfigure.width;
            unsigned height = ev.xconfigure.height;
            if (width != m_width || height != m_height)
            {
                resize(width, height);
                m_target.render();
            }
        }
        break;

        case Expose:
            /* if count is > 0 then ignore the event because
               other expose event will follow */
            if (ev.xexpose.count == 0)
            {
                m_target.draw();
                XFlush(xc->display);
                XSync(xc->display, false);
                break;
            }

        case ClientMessage:
            if (ev.xclient.format == 32 && ev.xclient.data.l[0] == int(m_close_atom))
            {
                quit = true;
            }
            break;
        }
        m_mutex.unlock();
    }
}

void xwindow::close()
{
    free_x_resources();
    close_connections();
}

void xwindow::update_region(graphics::image& src_img, const agg::rect_i& r)
{
    const unsigned width = r.x2 - r.x1, height = r.y2 - r.y1;

    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, src_img, r, graphics::image::pixel_size);

    m_draw_img->x_resize(width, height);

    rendering_buffer_copy(*m_draw_img, m_sys_format, src_view, (agg::pix_format_e) graphics::pixel_format);

    Display *dsp = m_draw_conn.display;

    int x_dst = r.x1, y_dst = (graphics::flip_y ? src_img.height() -r.y2 : r.y1);
    XPutImage(dsp, m_window, m_gc, m_draw_img->ximage(),
              0, 0, x_dst, y_dst, width, height);
}
