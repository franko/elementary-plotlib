#pragma once

#include <fx.h>

#include "render_config.h"
#include "window_surface.h"
#include "status_notifier.h"
#include "window_flags.h"
#include "update_region_info.h"

class PlotCanvas;

class window_fox : public graphics::display_window {
public:
    window_fox(graphics::render_target& tgt, PlotCanvas *canvas);
    ~window_fox();

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);
    virtual void update_region_request(graphics::image& img, const agg::rect_i& r);

    virtual void lock()   { m_mutex.lock();   }
    virtual void unlock() { m_mutex.unlock(); }
    virtual int status() { return m_window_status.value(); }

    int set_notify_request(notify_request<graphics::window_status_e>& request) {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_window_status.set_notify_request(request);
    }

    void call_update_region() {
        update_region(*m_update_region.img, m_update_region.r);
        m_update_notify.notify();
    }

    void draw(FXEvent *ev) {
        m_target.draw();
    }

private:
    FXApp *app();

    PlotCanvas *m_fx_canvas;
    FXGUISignal *m_gui_signal;
    update_region_info   m_update_region;
    update_region_notify m_update_notify;
    std::mutex m_mutex;
    status_notifier<graphics::window_status_e> m_window_status;
    graphics::render_target& m_target;
};
