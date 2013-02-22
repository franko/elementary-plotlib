#ifndef GRAPH_MUTEX_H
#define GRAPH_MUTEX_H

#include <pthread.h>

struct graph_mutex {
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

class graph_mutex_simple : public graph_mutex {
public:
    graph_mutex_simple()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }

    virtual void lock()   { pthread_mutex_lock(&m_mutex);   }
    virtual void unlock() { pthread_mutex_unlock(&m_mutex); }

private:
    pthread_mutex_t m_mutex;
};

#endif
