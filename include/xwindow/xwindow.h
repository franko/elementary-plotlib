#pragma once

#include <mutex>

#include "debug_priv.h"
#include "display_window_status.h"
// Used for declarations of window_flag_e and pix_format_e enums.
// Not used for platform_support class which is not used in this project.
#include "platform/agg_platform_support.h"
#include "start_window.h"
#include "strpp.h"
#include "update_region_notify.h"
#include "window_surface.h"
#include "xwindow/x_connection.h"
#include "xwindow/x_image.h"

class xwindow : public graphics::display_window_status {
public:
    enum xevent_mask_e
    {
        xevent_mask = ExposureMask | StructureNotifyMask,
    };

    xwindow(graphics::window_surface& window_surface);
    ~xwindow();

    void start_blocking(unsigned width, unsigned height, unsigned flags);

    void start(unsigned width, unsigned height, unsigned flags) {
        int status = start_window_new_thread(this, width, height, flags);
        if (status != 0) {
            debug_log(1, "error starting window, unknwon error");
        }
    }

    bool send_request(graphics::window_request request_type, int plot_index) override;

private:
    void lock() {
        m_mutex.lock();
    }

    void unlock() {
        m_mutex.unlock();
    }

    void update_region(const graphics::image& src_img, const agg::rect_i& r) override;
    bool init(unsigned width, unsigned height, unsigned flags);
    void run();
    void close();

    bool send_update_region_event();
    bool send_close_window_event();
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
    Atom                 m_update_region_atom;
    x_connection         m_connection;
    x_connection         m_request_connection;
    x_image*             m_draw_img;
    str                  m_caption;
    update_region_notify m_update_notify;
    graphics::window_surface& m_window_surface;
    std::mutex           m_mutex;
};
