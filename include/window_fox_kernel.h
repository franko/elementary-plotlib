#pragma once

#include <fx.h>
#include <thread>

#include "display_window_status.h"
#include "update_region_info.h"

class window_fox_kernel : public graphics::display_window_status {
public:
    window_fox_kernel(graphics::window_surface& window_surface);
    ~window_fox_kernel();

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);
    virtual void update_region_request(graphics::image& img, const agg::rect_i& r);

    void start(unsigned width, unsigned height, unsigned flags);

    void draw(FXEvent *ev) {
        m_window_surface.draw();
    }

    void on_resize(FXint w, FXint h) {
        if (w <= 0 || h <= 0) return;
        m_window_surface.resize(unsigned(w), unsigned(h));
        m_window_surface.render();
    }

    void set_thread_id() {
        m_window_thread_id = std::this_thread::get_id();
    }

    void call_update_region() {
        update_region(*m_update_region.img, m_update_region.r);
        m_update_notify.notify();
    }

    void bind_drawable(FXDrawable *drawable, FXSelector update_selector);
    void bind_window_environment(FXApp *app, FXObject *env_object, FXSelector start_selector);

private:
    FXDrawable *m_drawable;
    FXGUISignal *m_update_signal, *m_start_signal;
    update_region_info   m_update_region;
    update_region_notify m_update_notify;
    std::thread::id m_window_thread_id; // Identifies the thread that manage the Window's event loop.
    graphics::window_surface& m_window_surface;
};
