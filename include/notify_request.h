#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

enum request_error_e {
    request_success            = 0,
    request_error_pending      = -1,
    request_error_unknown      = -2,
    request_satisfied          = -3,
};

template <typename StatusOrderedEnum>
class notify_request {
public:
    notify_request(StatusOrderedEnum req): m_request(req), m_completed(false) { }

    StatusOrderedEnum value() const { return m_request; }

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
    const StatusOrderedEnum m_request;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_completed;
};
