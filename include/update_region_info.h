#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>

#include <agg_basics.h>

#include "window_surface.h"

// Used to send un update_region request from the caller thread to the
// window's thread. Includes condition variable to wait for the request
// to be treated.
struct update_region_info {
    graphics::image *img;
    agg::rect_i r;

    update_region_info(): img(nullptr) { }

    void prepare(graphics::image& img_, agg::rect_i r_) {
        img = &img_;
        r = r_;
    }

    void clear() {
        img = nullptr;
    }
};

struct update_region_notify {
    std::mutex mutex;
    std::condition_variable condition;
    bool completed;

    update_region_notify(): completed(false) { }

    void prepare() {
        completed = false;
    }

    void notify() {
        mutex.lock();
        completed = true;
        mutex.unlock();
        condition.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lk(mutex);
        if (!completed) {
            condition.wait(lk, [this] { return this->completed; });
        }
    }
};
