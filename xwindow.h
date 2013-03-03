#ifndef XWINDOW_XWINDOW_H
#define XWINDOW_XWINDOW_H

#include "platform/agg_platform_support.h"

#include "x_connection.h"
#include "strpp.h"
#include "pthreadpp.h"

struct render_target {
    virtual void resize(unsigned width, unsigned height) = 0;
    virtual void draw() = 0;
};

class xwindow {
public:
    //----------------------------------------------------------window_flag_e
    // These are flags used in method init(). Not all of them are
    // applicable on different platforms, for example the win32_api
    // cannot use a hardware buffer (window_hw_buffer).
    // The implementation should simply ignore unsupported flags.
    enum window_flag_e
    {
        window_resize            = 1,
        window_hw_buffer         = 2,
        window_keep_aspect_ratio = 4,
        window_process_all_keys  = 8,
    };

    enum {
        window_min_width  = 128,
        window_min_height = 128,
        window_max_width  = 4096,
        window_max_height = 4096,
    };

    enum xevent_mask_e
    {
        xevent_mask = ExposureMask | StructureNotifyMask,
    };

    xwindow(render_target& tgt, agg::pix_format_e format, bool flip_y);

    bool init(unsigned width, unsigned height, unsigned flags);
    void run();

    void close_connections();
    void caption(const str& s);
    void wait_map_notify();
    void free_x_resources();
    void resize(unsigned width, unsigned height);

private:
    unsigned             m_window_flags;

    agg::pix_format_e    m_format;
    agg::pix_format_e    m_sys_format;
    int                  m_byte_order;
    bool                 m_flip_y;
    unsigned             m_bpp;
    unsigned             m_sys_bpp;

    unsigned             m_width;
    unsigned             m_height;

    Window               m_window;
    GC                   m_gc;
    XSetWindowAttributes m_window_attributes;
    Atom                 m_close_atom;
    Atom                 m_wm_protocols_atom;

    x_connection         m_main_conn;
    x_connection         m_draw_conn;

    str                  m_caption;
    pthread::mutex       m_mutex;

    render_target&       m_target;
};

#endif
