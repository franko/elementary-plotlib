#ifndef WINDOW_THREAD_H
#define WINDOW_THREAD_H

#include "pthreadpp.h"
#include "window_flags.h"

template <typename Window>
class window_thread : public pthread::thread {
public:
    window_thread(Window& win): m_window(win) {}

    void start(unsigned width, unsigned height, unsigned flags) {
        m_window.init(width, height, flags);
        pthread::thread::start();
    }

    virtual void run() {
        m_window.run();
        m_window.close();
    }

private:
    Window& m_window;
};

#endif
