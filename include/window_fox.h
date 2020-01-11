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
    window_fox(FXElpWindow *canvas, const char *split_str): window_fox_kernel(m_surface), m_surface(split_str) {
        bind_drawable(canvas, FXElpWindow::ID_UPDATE_REGION);
        m_surface.attach_window(this);
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
