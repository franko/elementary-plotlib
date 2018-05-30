#pragma once

#include "render_config.h"
#include "window_surface.h"
#include "status_notifier.h"
#include "window_flags.h"

class FXCanvas;
class FXApp;

class window_fox : public graphics::display_window {
public:
    window_fox(graphics::render_target& tgt, FXCanvas *canvas);
    ~window_fox() { }

    void start(unsigned width, unsigned height, unsigned flags);

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);
    virtual void update_region_request(graphics::image& img, const agg::rect_i& r);

    virtual void lock()   { m_mutex.lock();   }
    virtual void unlock() { m_mutex.unlock(); }
    virtual int status() { return m_window_status.value(); }

    int set_notify_request(notify_request<graphics::window_status_e>& request) {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_window_status.set_notify_request(request);
    }

private:
    FXCanvas *m_fx_canvas;
    update_region_info   m_update_region;
    update_region_notify m_update_notify;
    std::mutex m_mutex;
    status_notifier<graphics::window_status_e> m_window_status;
    graphics::render_target& m_target;

    static FXApp *g_app;
};