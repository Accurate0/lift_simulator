#include <stdio.h>
#include <stdlib.h>

#include "log.h"

struct logger* logger_init(FILE *fp)
{
    struct logger *l = malloc(sizeof(struct logger));

    l->fp = fp;
    pthread_mutex_init(&l->mutex, NULL);

    return l;
}

void logger_free(struct logger *l)
{
    // File will be closed by whoever opened it
    pthread_mutex_destroy(&l->mutex);
    free(l);
}
