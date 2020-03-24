#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

struct mutex {
    pthread_mutex_t q_mutex;
    pthread_cond_t q_full;
};

#endif
