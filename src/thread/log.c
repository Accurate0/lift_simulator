#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

struct log* log_init(FILE *fp)
{
    struct log *l = malloc(sizeof(struct log));

    l->fp = fp;
    pthread_mutex_init(&l->mutex, NULL);

    return l;
}

void log_free(struct log *l)
{
    // File will be closed by whoever opened it
    // eg. the main thread in this case
    pthread_mutex_destroy(&l->mutex);
    free(l);
}

void log_printf(struct log *l, const char *s, ...)
{
    va_list args;
    va_start(args, s);

    pthread_mutex_lock(&l->mutex);
    vfprintf(l->fp, s, args);
    pthread_mutex_unlock(&l->mutex);

    va_end(args);
}
