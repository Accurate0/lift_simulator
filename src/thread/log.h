#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <pthread.h>

struct log {
    pthread_mutex_t mutex;
    FILE *fp;
};

struct log* log_init(FILE *fp);
void log_free(struct log *l);
void log_printf(struct log *l, const char *s, ...);

#endif
