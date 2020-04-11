#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <pthread.h>

struct logger {
    pthread_mutex_t mutex;
    FILE *fp;
};

struct logger* logger_init(FILE *fp);
void logger_free(struct logger *l);

#define FILE_LOG(l, s, ...)               \
    do {                                  \
        pthread_mutex_lock(&l->mutex);    \
        fprintf(l->fp, s, __VA_ARGS__);   \
        pthread_mutex_unlock(&l->mutex);  \
    } while(0)

#endif
