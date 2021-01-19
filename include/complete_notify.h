#pragma once

#include <mutex>
#include <condition_variable>

template <typename MessageType>
class complete_notify {
public:
    complete_notify(): m_completed(true) { }

    void start(const MessageType& message) {
        m_completed = false;
        m_message = message;
    }

    void notify() {
        m_mutex.lock();
        m_completed = true;
        m_mutex.unlock();
        m_condition.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lk(m_mutex);
        if (!m_completed) {
            m_condition.wait(lk, [this] { return this->m_completed; });
        }
    }

    bool completed() const { return m_completed; }
    MessageType& message() { return m_message; }

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_completed;
    MessageType m_message;
};

