#ifndef XWINDOW_XWINDOW_H
#define XWINDOW_XWINDOW_H

#include <thread>
#include <mutex>
#include <condition_variable>

// Used for declarations of window_flag_e and pix_format_e enums.
// Not used for platform_support class which is not used in this project.
#include "platform/agg_platform_support.h"

#include "xwindow/x_connection.h"
#include "xwindow/x_image.h"
#include "strpp.h"
#include "window_surface.h"
#include "window_flags.h"

class xwindow : public graphics::display_window {
public:
    enum xevent_mask_e
    {
        xevent_mask = ExposureMask | StructureNotifyMask,
    };

    xwindow(graphics::render_target& tgt);
    ~xwindow();

    void start(unsigned width, unsigned height, unsigned flags);
    void wait_running(std::unique_lock<std::mutex>& lock);
    std::unique_lock<std::mutex> get_lock() { return std::unique_lock<std::mutex>(m_mutex); }

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);

    virtual void lock()   { m_mutex.lock();   }
    virtual void unlock() { m_mutex.unlock(); }

private:
    bool init(unsigned width, unsigned height, unsigned flags);
    void run();
    void close();

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
    int                  m_window_status;
    std::mutex           m_mutex;
    std::condition_variable m_running;
    graphics::render_target& m_target;
};

#endif
