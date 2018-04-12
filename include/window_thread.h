#ifndef WINDOW_THREAD_H
#define WINDOW_THREAD_H

#include "pthreadpp.h"
#include "window_flags.h"

template <typename Window>
struct window_thread : public pthread::thread {
    window_thread(Window& win): m_window(win) {}
    virtual void run() {
        m_window.init(640, 480, graphics::window_resize);
        m_window.run();
        m_window.close();
    }
private:
    Window& m_window;
};

#endif
