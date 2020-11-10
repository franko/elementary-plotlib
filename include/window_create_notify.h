#pragma once

#include <mutex>
#include <condition_variable>

struct window_create_message {
    const char *caption;
    unsigned width;
    unsigned height;
    unsigned flags;
    window_close_callback *close_callback; // FIXME: remove callback from here.
};

/* Modeled after update_window_notify. Consider merge them in
   a single implementation. */
struct window_create_notify {
    std::mutex mutex;
    std::condition_variable condition;
    bool completed;
    window_create_message message;
    void *new_window;

    window_create_notify(): completed(true) { }

    void start(const char *caption, unsigned width, unsigned height, unsigned flags, window_close_callback *close_callback) {
        completed = false;
        message = window_create_message{caption, width, height, flags, close_callback};
        new_window = nullptr;
    }

    void notify(void *window) {
        mutex.lock();
        completed = true;
        new_window = window;
        mutex.unlock();
        condition.notify_one();
    }

    void *wait() {
        std::unique_lock<std::mutex> lk(mutex);
        if (!completed) {
            condition.wait(lk, [this] { return this->completed; });
        }
        return new_window;
    }
};
