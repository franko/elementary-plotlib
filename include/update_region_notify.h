#pragma once

#include <mutex>
#include <condition_variable>

struct update_region_notify {
    std::mutex mutex;
    std::condition_variable condition;
    bool completed;
    int plot_index;

    update_region_notify(): completed(true) { }

    void start(int plot_index_) {
        completed = false;
        plot_index = plot_index_;
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
