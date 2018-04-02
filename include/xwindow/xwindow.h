#ifndef XWINDOW_XWINDOW_H
#define XWINDOW_XWINDOW_H

// Used for declarations of window_flag_e and pix_format_e enums.
// Not used for platform_support class which is not used in this project.
#include "platform/agg_platform_support.h"

#include "xwindow/x_connection.h"
#include "xwindow/x_image.h"
#include "strpp.h"
#include "pthreadpp.h"
#include "window_surface.h"

class xwindow : public graphics::display_window {
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

    xwindow(graphics::render_target& tgt);
    ~xwindow();

    bool init(unsigned width, unsigned height, unsigned flags);
    void run();
    void close();

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);

    virtual void lock()   { m_mutex.lock();   }
    virtual void unlock() { m_mutex.unlock(); }

private:
    void close_connections();
    void caption(const str& s);
    void wait_map_notify();
    void free_x_resources();
    void resize(unsigned width, unsigned height);

    unsigned             m_window_flags;
    agg::pix_format_e    m_sys_format;
    int                  m_byte_order;
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
    x_image*             m_draw_img;
    str                  m_caption;
    pthread::mutex       m_mutex;
    graphics::render_target& m_target;
};

#endif
