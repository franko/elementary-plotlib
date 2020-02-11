#pragma once

#include <mutex>
#include <condition_variable>

enum class update_status {
    waiting,
    retry,
    completed,
};

struct update_region_notify {
    std::mutex mutex;
    std::condition_variable condition;
    update_status status;

    update_region_notify(): status(update_status::completed) { }

    void start() {
        status = update_status::waiting;
    }

    void notify(update_status new_status) {
        mutex.lock();
        status = new_status;
        mutex.unlock();
        condition.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lk(mutex);
        if (status == update_status::waiting) {
            condition.wait(lk, [this] { return status != update_status::waiting; });
        }
    }
};
