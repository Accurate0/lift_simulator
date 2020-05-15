/**
 * @file log.c
 * @author Anurag Singh (18944183)
 *
 * @date 09-05-20
 *
 * Handles log struct creation
 * and mutual exclusion for logging
 * to file
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

/**
 * Creates a log struct with
 * file pointer fp, initialises
 * the mutex used to protect FILE io
 *
 * @param fp
 * @return struct log*
 */
struct log* log_init(FILE *fp)
{
    struct log *l = malloc(sizeof(struct log));

    l->fp = fp;
    pthread_mutex_init(&l->mutex, NULL);

    return l;
}

/**
 * Frees the log struct, and destroys the mutex
 *
 * @param l log struct to be freed
 */
void log_free(struct log *l)
{
    // File will be closed by whoever opened it
    // eg. the main thread in this case
    pthread_mutex_destroy(&l->mutex);
    free(l);
}

/**
 * printf like function that allows printing
 * to the FILE in the log struct, while utilising
 * the mutex to protect it
 *
 * @param l
 * @param s
 * @param ...
 */
void log_printf(struct log *l, const char *s, ...)
{
    va_list args;
    va_start(args, s);

    pthread_mutex_lock(&l->mutex);
    vfprintf(l->fp, s, args);
    pthread_mutex_unlock(&l->mutex);

    va_end(args);
}
