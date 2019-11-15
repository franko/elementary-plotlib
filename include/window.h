#pragma once

#include "window_surface.h"
#include "notify_request.h"
#include "window_flags.h"
#include "debug_priv.h"

template <typename Window>
void run_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    window->start(width, height, flags);
    window->unlock();
}

template <typename Window>
void wait_until_notification(Window *window, graphics::window_status_e notify_status) {
    notify_request<graphics::window_status_e> req{notify_status};
    int retval = window->set_notify_request(req);
    if (retval == request_success) {
        req.wait();
    } else if (retval == request_satisfied) {
        debug_log(1, "request already satisfied");
    } else {
        debug_log(1, "error sending request: %d", retval);
    }
}

template <typename Window>
void start_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    std::thread wt(run_window<Window>, window, width, height, flags);
    window->unlock();
    wt.detach();
    wait_until_notification(window, graphics::window_running);
}

template <typename Window>
class window_gen {
public:
    window_gen(): m_surface(nullptr), m_window(m_surface) {
        m_surface.attach_window(&m_window);
    }

	window_gen(const char *fmt): m_surface(fmt), m_window(m_surface) {
		m_surface.attach_window(&m_window);
	}

    int attach(graphics::plot* p, const char* slot_str) {
    	return m_surface.attach(p, slot_str);
    }

    void slot_refresh(unsigned index) {
    	m_surface.slot_refresh(index);
    }

	void start(unsigned width, unsigned height, unsigned flags) {
        start_window(&m_window, width, height, flags);
	}

    void wait() {
        wait_until_notification(&m_window, graphics::window_closed);
    }

    graphics::window_surface *window_surface() {
        return &m_surface;
    }

private:
	graphics::window_surface m_surface;
	Window m_window;
};

#if defined (WIN32) || defined (CYGWIN)
#include "win32/window_win32.h"
typedef window_gen<window_win32> window_platform_native;
#else
#include "xwindow/xwindow.h"
typedef window_gen<xwindow> window_platform_native;
#endif

namespace graphics {

class window : public window_platform_native {
public:
    using window_platform_native::window_platform_native;
};

}
