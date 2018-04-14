#ifndef LIBCANVAS_WINDOW_H
#define LIBCANVAS_WINDOW_H

#include "window_surface.h"
#include "window_thread.h"

template <typename Window>
class window_gen {
public:
	window_gen(const char *fmt): m_surface(fmt), m_window(m_surface), m_thread(m_window) {
		m_surface.attach_window(&m_window);
	}

    int attach(graphics::plot* p, const char* slot_str) {
    	return m_surface.attach(p, slot_str);
    }

    void slot_refresh(unsigned index) {
    	m_surface.slot_refresh(index);
    }

	void start(unsigned width, unsigned height, unsigned flags) {
		m_thread.start(width, height, flags);
	}

private:
	graphics::window_surface m_surface;
	Window m_window;
	window_thread<Window> m_thread;
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
