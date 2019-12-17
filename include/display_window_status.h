#pragma once

#include <mutex>

#include "display_window.h"
#include "notify_request.h"
#include "status_notifier.h"
#include "window_flags.h"

namespace graphics {

/* Extend the interface 'display_window'
 * by providing a mutex for lock/unlock methods and a status variable
 * with the related methods to query/set status and issue a request. */
class display_window_status : public display_window {
public:
    void lock() {
        m_mutex.lock();
    }
    void unlock() {
        m_mutex.unlock();
    }
    int status() {
        return m_window_status.value();
    }
    void set_status(window_status_e new_status) {
        m_window_status.set(new_status);
    }
    int set_notify_request(notify_request<window_status_e>& request) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_window_status.set_notify_request(request);
    }
private:
    std::mutex m_mutex;
    status_notifier<window_status_e> m_window_status;
};

}
