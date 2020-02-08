#include <X11/X.h>
#include <X11/Xutil.h>

#include "notify_request.h"
#include "xwindow/xwindow.h"
#include "debug_priv.h"
#include "fatal.h"

bool xwindow::need_initialize = true;

// m_sys_format, m_byte_order, m_sys_bpp will be set based on XWindow display on "init" method.
xwindow::xwindow(graphics::window_surface& window_surface):
    m_sys_format(agg::pix_format_undefined),
    m_byte_order(LSBFirst),
    m_sys_bpp(0),
    m_width(0), m_height(0),
    m_window(0),
    m_gc(0),
    m_close_atom(0),
    m_wm_protocols_atom(0),
    m_update_region_atom(0),
    m_draw_img(0),
    m_caption("Graphics Window"),
    m_window_surface(window_surface)
{
}

xwindow::~xwindow()
{
    delete m_draw_img;
}

void xwindow::close_connections()
{
    m_connection.close();
}

void xwindow::free_x_resources()
{
    XFreeGC(m_connection.display, m_gc);
}

void xwindow::caption(const str& text)
{
    // Fixed by Enno Fennema (in original AGG library)
    const char *capt = text.cstr();
    Display *d = m_connection.display;
    XStoreName(d, m_window, capt);
    XSetIconName(d, m_window, capt);
}

bool xwindow::init(unsigned width, unsigned height, unsigned flags)
{
    if (need_initialize) {
        XInitThreads();
        need_initialize = false;
    }

    m_window_flags = flags;

    if (unlikely(!m_connection.init()))
        return false;

    set_status(graphics::window_starting);
    x_connection *xc = &this->m_connection;

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
            hints->min_width  = graphics::window_min_width;
            hints->min_height = graphics::window_min_height;
            hints->max_width  = graphics::window_max_width;
            hints->max_height = graphics::window_max_height;
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

    m_close_atom = XInternAtom(xc->display, "WM_DELETE_WINDOW", False);
    m_wm_protocols_atom = XInternAtom(xc->display, "WM_PROTOCOLS", True);
    m_update_region_atom = XInternAtom(xc->display, "UPDATE_REGION_REQUEST", False);
    XSetWMProtocols(xc->display, m_window, &m_close_atom, 1);

    wait_map_notify();
    resize(width, height);
    m_window_surface.render();

    set_status(graphics::window_running);

    return true;
}

void xwindow::wait_map_notify()
{
    x_connection *xc = &this->m_connection;
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
    m_window_surface.resize(width, height);

    delete m_draw_img;
    m_draw_img = new(std::nothrow) x_image(m_sys_bpp, m_byte_order, width, height, &m_connection);

    m_width = width;
    m_height = height;
}

void xwindow::run()
{

    x_connection *xc = &this->m_connection;

    bool quit = false;

    while (!quit)
    {
        XEvent ev;

        unlock();
        XNextEvent(xc->display, &ev);
        lock();

        switch(ev.type)
        {
        case ConfigureNotify:
        {
            unsigned width  = ev.xconfigure.width;
            unsigned height = ev.xconfigure.height;
            if (width != m_width || height != m_height)
            {
                resize(width, height);
                m_window_surface.render();
            }
        }
        break;

        case Expose:
            /* if count is > 0 then ignore the event because
               other expose event will follow */
            if (ev.xexpose.count == 0)
            {
                // FIXME: this code is copy & pasted in three different places,
                // all the window's implementations.
                graphics::window_surface::image_guard guard(m_window_surface);
                const graphics::image &surface_image = m_window_surface.get_image(guard);
                const agg::rect_i r(0, 0, surface_image.width(), surface_image.height());
                update_region(surface_image, r);
                break;
            }

        case ClientMessage:
            if (ev.xclient.message_type == m_wm_protocols_atom && ev.xclient.format == 32 && ev.xclient.data.l[0] == int(m_close_atom)) {
                quit = true;
            } else if (ev.xclient.message_type == m_update_region_atom && m_update_region.img) {
                update_region(*m_update_region.img, m_update_region.r);
                m_update_notify.notify();
            }
            break;
        }
    }
}

void xwindow::close()
{
    free_x_resources();
    close_connections();
    set_status(graphics::window_closed);
}

void xwindow::update_region(const graphics::image& src_img, const agg::rect_i& r)
{
    const unsigned width = r.x2 - r.x1, height = r.y2 - r.y1;

    rendering_buffer_ro src_view;
    rendering_buffer_get_const_view(src_view, src_img, r, graphics::image::pixel_size);

    m_draw_img->x_resize(width, height);

    rendering_buffer_copy(*m_draw_img, m_sys_format, src_view, (agg::pix_format_e) graphics::pixel_format);

    Display *dsp = m_connection.display;

    int x_dst = r.x1, y_dst = (graphics::flip_y ? src_img.height() -r.y2 : r.y1);
    XPutImage(dsp, m_window, m_gc, m_draw_img->ximage(),
              0, 0, x_dst, y_dst, width, height);
}

void xwindow::start_blocking(unsigned width, unsigned height, unsigned flags) {
    lock();
    init(width, height, flags);
    run();
    close();
    unlock();
}

void xwindow::update_region_request(graphics::image& img, const agg::rect_i& r) {
    m_update_notify.clear();
    m_update_region.prepare(img, r);
    m_update_notify.completed = false;
    if (send_update_region_event()) {
        // Wait for the notification but only if the message was actually sent.
        m_update_notify.wait();
    }
    m_update_region.clear();
}

// Returns true is the message was actually sent.
bool xwindow::send_update_region_event() {
    lock();
    if (status() == graphics::window_running) {
        XClientMessageEvent event;
        event.type = ClientMessage;
        event.display = m_connection.display;
        event.send_event = True;
        event.message_type = m_update_region_atom;
        event.format = 8;
        Status status = XSendEvent(m_connection.display, m_window, False, NoEventMask, (XEvent *) &event);
        unlock();
        if (status == BadValue) {
            debug_log(1, "custom event, got BadValue");
        } else if (status == BadWindow) {
            debug_log(1, "custom event, got BadWindow");
        } else {
            XFlush(m_connection.display);
        }
        return true;
    }
    unlock();
    return false;
}
