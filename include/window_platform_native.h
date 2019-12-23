#pragma once

#include <thread>

#include "window.h"
#include "window_surface.h"
#include "window_flags.h"
#include "notify_request.h"
#include "debug_priv.h"

template <typename Window>
void run_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    window->start(width, height, flags);
    window->unlock();
}

template <typename Window>
void start_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    std::thread wt(run_window<Window>, window, width, height, flags);
    window->unlock();
    wt.detach();
    request_error_e status = window->wait_until_notification(graphics::window_running);
    if (status != request_satisfied && status != request_success) {
        debug_log(1, "error starting window, error code: %d", int(status));
    }
}

template <typename Window>
class window_gen : public elp_window {
public:
    window_gen(): m_surface(nullptr), m_window(m_surface) {
        m_surface.attach_window(&m_window);
    }

    window_gen(const char *fmt): m_surface(fmt), m_window(m_surface) {
        m_surface.attach_window(&m_window);
    }

    void set_layout(const char *fmt) override {
        if (m_window.status() == graphics::window_not_started) {
            m_surface.split(fmt);
        }
    }

    int attach(graphics::plot* p, const char* slot_str) override {
        return m_surface.attach(p, slot_str);
    }

    void slot_refresh(unsigned index) override {
        m_surface.slot_refresh(index);
    }

    void start(unsigned width, unsigned height, unsigned flags) override {
        start_window(&m_window, width, height, flags);
    }

    void wait() override {
        request_error_e status = m_window.wait_until_notification(graphics::window_closed);
        if (status != request_satisfied && status != request_success) {
            debug_log(1, "error while waiting window's closing, error code: %d", int(status));
        }
    }

    graphics::window_surface *get_window_surface() override {
        return &m_surface;
    }

private:
    graphics::window_surface m_surface;
    Window m_window;
};

#if defined (WIN32) || defined (CYGWIN)
#include "win32/window_win32.h"
typedef window_gen<window_win32> elp_window_native;
#else
#include "xwindow/xwindow.h"
typedef window_gen<xwindow> elp_window_native;
#endif
