#pragma once

#include <cassert>
#include <condition_variable>
#include <mutex>

enum task_status { kTaskRunning = 0, kTaskComplete };

template <typename StatusOrderedEnum>
class status_notifier {
public:
    status_notifier(): m_status(StatusOrderedEnum(0)), m_request_pending(false) { }

    // Protection may be needed.
    StatusOrderedEnum value() const { return m_status; }

    // Set the status and send notification to requester, if there is one and
    // is waiting for the given code.
    void set(StatusOrderedEnum new_status) {
        m_mutex.lock();
        m_status = new_status;
        if (m_request_pending && m_request_status == new_status) {
            m_condition.notify_one();
            m_mutex.unlock(); // Valgrid seems to suggest it should go after the notify.
        } else {
            m_mutex.unlock();
        }
    }

    void wait_for_status(StatusOrderedEnum request) {
        std::unique_lock<std::mutex> lk(m_mutex);
        assert(!m_request_pending);
        m_request_status = request;
        if (m_status < request) {
            m_request_pending = true;
            m_condition.wait(lk, [this] { return this->m_status >= this->m_request_status; });
            m_request_pending = false;
        }
    }

private:
    StatusOrderedEnum m_status;
    bool m_request_pending;
    StatusOrderedEnum m_request_status;
    std::condition_variable m_condition;
    std::mutex m_mutex;
};
