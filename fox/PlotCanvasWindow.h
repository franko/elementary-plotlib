#pragma once

class window_fox {
public:
    window_fox(graphics::render_target& tgt);
    ~window_fox();

    void start(unsigned width, unsigned height, unsigned flags);

    virtual void update_region(graphics::image& src_img, const agg::rect_i& r);

    virtual void lock()   { m_mutex.lock();   }
    virtual void unlock() { m_mutex.unlock(); }
    virtual int status() { return m_window_status.value(); }

    int set_notify_request(notify_request<graphics::window_status_e>& request) {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_window_status.set_notify_request(request);
    }

private:
    FXMainWindow *m_main_window;
    std::mutex m_mutex;
    status_notifier<graphics::window_status_e> m_window_status;
    graphics::render_target& m_target;
};
