#pragma once

#include <fx.h>
#include <thread>

#include "display_window_status.h"
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

    update_status update_region_request(int index) override;
    void update_region(const graphics::image& src_img, const agg::rect_i& r) override;

    void start(unsigned width, unsigned height, unsigned flags);

    void draw(FXEvent *ev) {
        // FIXME: this code is copy & pasted in three different places,
        // all the window's implementations.
        graphics::window_surface::image_guard guard(m_window_surface);
        const graphics::image &surface_image = m_window_surface.get_image(guard);
        const agg::rect_i r(0, 0, surface_image.width(), surface_image.height());
        update_region(surface_image, r);
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
        m_update_notify.notify(update_status::completed);
    }

    void bind_drawable(FXDrawable *drawable, FXSelector update_selector);

private:

    FXDrawable *m_drawable;
    FXGUISignal *m_update_signal;
    FXGUISignal *m_start_signal; // Just a reference, not owned.
    update_region_notify m_update_notify;
    std::thread::id m_window_thread_id; // Identifies the thread that manage the Window's event loop.
    graphics::window_surface& m_window_surface;
};
