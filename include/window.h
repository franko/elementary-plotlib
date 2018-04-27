#ifndef LIBCANVAS_WINDOW_H
#define LIBCANVAS_WINDOW_H

#include "window_surface.h"
#include "notify_request.h"
#include "debug_log.h"

template <typename Window>
void run_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    window->start(width, height, flags);
    window->unlock();
}

template <typename Window>
void wait_until_notification(Window *window, notify_e notify_code) {
    notify_request req{notify_code};
    int retval = window->send_notify_request(req);
    if (retval == request_success) {
        req.wait();
    } else if (retval == request_not_applicable) {
        debug_log("request already satisfied");
    } else {
        debug_log("error sending request: %d", retval);
    }
}

template <typename Window>
void start_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    std::thread wt(run_window<Window>, window, width, height, flags);
    window->unlock();
    wt.detach();
    wait_until_notification(window, notify_window_start);
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
        wait_until_notification(&m_window, notify_window_closed);
    }

private:
	graphics::window_surface m_surface;
	Window m_window;
};

#ifdef WIN32
#include "win32/window_win32.h"
namespace graphics {
typedef window_gen<window_win32> window;
}
#else
#include "xwindow/xwindow.h"
namespace graphics {
typedef window_gen<xwindow> window;
}
#endif

#endif
