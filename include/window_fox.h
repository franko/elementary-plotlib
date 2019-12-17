#pragma once

#include <fx.h>

#include "debug_priv.h"
#include "display_window_status.h"
#include "update_region_info.h"
#include "window_surface.h"

namespace FX {
    class FXElpWindow;
}

class window_fox : public graphics::display_window_status {
public:
    window_fox(FXElpWindow *canvas, const char *split_str);
    ~window_fox();

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);
    virtual void update_region_request(graphics::image& img, const agg::rect_i& r);

    void set_thread_id() {
        m_window_thread_id = std::this_thread::get_id();
    }

    void call_update_region() {
        update_region(*m_update_region.img, m_update_region.r);
        m_update_notify.notify();
    }

    void draw(FXEvent *ev) {
        debug_log(1, "window_fox: calling draw on target");
        m_surface.draw();
    }

    void on_resize(FXint w, FXint h) {
        if (w <= 0 || h <= 0) return;
        debug_log(1, "window_fox: calling resize on target %d %d", w, h);
        m_surface.resize(unsigned(w), unsigned(h));
        debug_log(1, "window_fox: calling render on target");
        m_surface.render();
    }

    int attach(graphics::plot* p, const char* slot_str) {
        return m_surface.attach(p, slot_str);
    }

    void slot_refresh(unsigned index) {
        m_surface.slot_refresh(index);
    }

    graphics::window_surface *window_surface() {
        return &m_surface;
    }

private:
    FXApp *app();

    FXElpWindow *m_plot_canvas;
    FXGUISignal *m_gui_signal;
    update_region_info   m_update_region;
    update_region_notify m_update_notify;
    std::thread::id m_window_thread_id; // Identifies the thread that manage the Window's event loop.
    graphics::window_surface m_surface;
};
