#ifndef LIBCANVAS_WINDOW_H
#define LIBCANVAS_WINDOW_H

#include <thread>
#include <mutex>

#include "window_surface.h"

template <typename Window>
void run_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    window->lock();
    window->init(width, height, flags);
    window->run();
    window->close();
    window->unlock();
}

template <typename Window>
void start_window(Window *window, unsigned width, unsigned height, unsigned flags) {
    auto lock = window->get_lock();
    std::thread wt(run_window<Window>, window, width, height, flags);
    window->wait_running(lock);
    lock.unlock();
    wt.detach();
}

template <typename Window>
class window_gen {
public:
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