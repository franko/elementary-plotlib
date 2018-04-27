#ifndef LIBCANVAS_NOTIFY_REQUEST_H
#define LIBCANVAS_NOTIFY_REQUEST_H

#include <thread>
#include <mutex>
#include <condition_variable>

enum notify_e {
    notify_window_start,
    notify_window_closed,
};

enum request_error_e {
    request_success            = 0,
    request_error_pending      = -1,
    request_error_unknown      = -2,
    request_not_applicable     = -3,
};

class notify_request {
public:
    notify_request(notify_e req): m_request(req), m_completed(false) { }

    notify_e type() const { return m_request; }

    void notify() {
        m_mutex.lock();
        m_completed = true;
        m_mutex.unlock();
        m_condition.notify_one();
    }

    void wait() {
         std::unique_lock<std::mutex> lk(m_mutex);
         m_condition.wait(lk, [this] { return this->m_completed; });
    }

private:
    const notify_e m_request;
    bool m_completed;
    std::mutex m_mutex;
    std::condition_variable m_condition;
};

#endif
