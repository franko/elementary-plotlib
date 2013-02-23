#ifndef GRAPH_MUTEX_H
#define GRAPH_MUTEX_H

#include <pthread.h>

namespace graphics {

struct mutex {
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

class global_mutex : public mutex {
public:
    global_mutex()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }

    virtual void lock()   { pthread_mutex_lock(&m_mutex);   }
    virtual void unlock() { pthread_mutex_unlock(&m_mutex); }

private:
    pthread_mutex_t m_mutex;
};

}

#endif
