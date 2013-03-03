#ifndef CPP_PTHREADPP_H
#define CPP_PTHREADPP_H

#include <pthread.h>

/* Simple C++ wrapper around basic mutex/condition operations.
   The added value of the wrapper is more clean C++ code with automatic
   initialization/deallocation of resources. */

namespace pthread {

class mutex {
public:
    mutex() {
        pthread_mutex_init(&m_mutex, NULL);
    }

    ~mutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

    pthread_mutex_t* mutex_ptr() {
        return &m_mutex;
    }

private:
    mutex(const mutex&);
    mutex& operator= (const mutex&);

    pthread_mutex_t m_mutex;
};

class cond {
public:
    cond() {
        pthread_cond_init(&m_cond, NULL);
    }

    ~cond() {
        pthread_cond_destroy(&m_cond);
    }

    void signal() {
        pthread_cond_signal(&m_cond);
    }

    void wait(mutex& mtx) {
        pthread_cond_wait(&m_cond, mtx.mutex_ptr());
    }

    void wait(pthread_mutex_t* mtx) {
        pthread_cond_wait(&m_cond, mtx);
    }

private:
    pthread_cond_t m_cond;
};
}

#endif
