#ifndef LIBCANVAS_STATUS_NOTIFIER_H
#define LIBCANVAS_STATUS_NOTIFIER_H

#include "notify_request.h"
#include "window_flags.h"

// Keep the status of an object (an ordered enum) and send a notification to
// a pending requester, if any, when a given status is set.
// The status enum is supposed to increse with the lifetime of the object.
template <typename StatusOrderedEnum>
class status_notifier {
public:
    status_notifier(): m_status(StatusOrderedEnum(0)), m_request_pending(nullptr) { }

    StatusOrderedEnum value() const { return m_status; }

    // Set the status and send notification to requester, if there is one and
    // is waiting for the given code.
    void set(StatusOrderedEnum new_status) {
        m_status = new_status;
        if (m_request_pending && m_request_pending->value() == new_status) {
            m_request_pending->notify();
            m_request_pending = nullptr;
        }
    }

    int set_notify_request(notify_request<StatusOrderedEnum>& request) {
        if (m_request_pending) {
            return request_error_pending;
        }
        if (m_status >= request.value()) {
            return request_satisfied;
        }
        m_request_pending = &request;
        return request_success;
    }

private:
    StatusOrderedEnum m_status;
    notify_request<StatusOrderedEnum> *m_request_pending;
};

#endif
