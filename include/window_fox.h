#pragma once

#include <fx.h>

#include "debug_priv.h"
#include "display_window_status.h"
#include "update_region_info.h"
#include "window_fox_kernel.h"
#include "window_surface.h"
#include "FXElpWindow.h"

class window_fox : public window_fox_kernel {
public:
    window_fox(FXElpWindow *canvas, const char *split_str): window_fox_kernel(), m_surface(split_str) {
        bind_drawable(canvas, FXSEL(SEL_IO_READ, FXElpWindow::ID_UPDATE_REGION));
        m_surface.attach_window(this);
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

    graphics::window_surface *get_window_surface() {
        return &m_surface;
    }

private:
    graphics::window_surface m_surface;
};
