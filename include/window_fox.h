#pragma once

#include <fx.h>
#include <thread>

#include "display_window_status.h"
#include "window_close_callback.h"
#include "update_region_notify.h"
#include "window_surface.h"

namespace FX {
class FXElemBaseWindow;
}

class window_fox : public graphics::display_window_status {
public:
    window_fox(graphics::window_surface& window_surface);
    window_fox(graphics::window_surface& window_surface, FXElemBaseWindow *elem_window);
    window_fox(graphics::window_surface& window_surface, FXGUISignal *start_signal);
    ~window_fox();

    bool send_request(graphics::window_request request_type, int plot_index) override;
    void update_region(const graphics::image& src_img, const agg::rect_i& r) override;

    void start(unsigned width, unsigned height, unsigned flags, window_close_callback *callback);

    void draw(FXEvent *ev) {
        m_window_surface.update_window_area();
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
        m_window_surface.slot_refresh(m_update_notify.plot_index);
        m_update_notify.notify();
    }

    void bind_elem_window(FXElemBaseWindow *elem_window);

private:

    FXElemBaseWindow *m_elem_window;
    FXGUISignal *m_update_signal;
    FXGUISignal *m_close_signal;
    FXGUISignal *m_start_signal; // Just a reference, not owned.
    update_region_notify m_update_notify;
    std::thread::id m_window_thread_id; // Identifies the thread that manage the Window's event loop.
    graphics::window_surface& m_window_surface;
};
