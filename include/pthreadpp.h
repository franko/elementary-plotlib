#ifndef CPP_PTHREADPP_H
#define CPP_PTHREADPP_H

#include <pthread.h>
#include "fatal.h"

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

/* Used to spawn a new thread. Should be subclassed by implementing
   the run() virtual method.
   Idea taken from the FOX toolkit, FXThread class */
class thread {
public:
    thread() { }
    virtual ~thread() { };

    void start() {
        pthread_attr_t attr[1];
        pthread_t thread[1];
        pthread_attr_init(attr);
        pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
        int rc = pthread_create(thread, attr, thread_run, (void *)this);
        if (rc) {
            fatal_exception("error creating thread");
        }
        pthread_attr_destroy(attr);
    }

    virtual void run() = 0;

private:
    static void* thread_run(void* _this) {
        thread* t = (thread*) _this;
        t->run();
        fprintf(stderr, "terminate.\n");
        pthread_exit(NULL);
        return NULL;
    }
};

}

#endif
