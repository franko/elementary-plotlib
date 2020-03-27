#pragma once

#include "elem_plot.h"
#include "elem_window.h"
#include "window_close_callback.h"
#include "window_flags.h"
#include "window_surface.h"
#include "debug_priv.h"

class elem_window_close_callback : public window_close_callback {
public:
    elem_window_close_callback(elem_window& window): m_window(window) { }
    void execute() override {
        m_window.release();
    }
private:
    elem_window& m_window;
};

template <typename Window>
class window_gen : public elem_window {
public:
    template <typename... Args>
    window_gen(Args&&... args): m_surface(nullptr), m_window(m_surface, std::forward<Args>(args)...) {
        m_surface.attach_window(&m_window);
    }

    template <typename... Args>
    window_gen(const char *fmt, Args&&... args): m_surface(fmt), m_window(m_surface, std::forward<Args>(args)...) {
        m_surface.attach_window(&m_window);
    }

    void set_layout(const char *fmt) override {
        if (m_window.status() == graphics::window_not_started) {
            m_surface.split(fmt);
        }
    }

    int attach(elem_plot* p, const char* slot_str) override {
        return m_surface.attach(p, slot_str);
    }

    void slot_refresh(unsigned index) override {
        m_surface.slot_refresh_request(index);
    }

    void start(unsigned width, unsigned height, unsigned flags) override {
        if (!graphics::global_fonts_initialized) {
            debug_log(1, "fonts not initialized, performing initialization");
            graphics::initialize_fonts();
        }
        retain();
        auto callback = new elem_window_close_callback{*this};
        m_window.start(width, height, flags, callback);
    }

    void wait() override {
        if (m_window.status() == graphics::window_running) {
            m_window.wait_for_status(graphics::window_closed);
        }
    }

    void close() override {
        m_window.send_request(graphics::window_request::close, 0);
    }

    graphics::window_surface *get_window_surface() override {
        return &m_surface;
    }
private:
    graphics::window_surface m_surface;
    Window m_window;
};
