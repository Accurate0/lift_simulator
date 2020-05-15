/**
 * @file scheduler.c
 * @author Anurag Singh (18944183)
 *
 * @date 09-05-20
 *
 * Handles all scheduler based actions
 * including allocating the structure
 * and the function passed to pthread_create
 *
 */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <common/file_io.h>
#include <common/request.h>
#include <common/debug.h>

#include "scheduler.h"

/**
 * Create a scheduler struct, initialising struct
 * fields to imported variables
 *
 * @param input
 * @param queue
 * @param logger
 * @return struct scheduler*
 */
struct scheduler* scheduler_init(FILE *input, struct queue *queue, struct log *logger)
{
    struct scheduler *s = malloc(sizeof(struct scheduler));
    s->total_requests = 0;
    s->input = input;
    s->logger = logger;
    s->queue = queue;

    return s;
}

/**
 * Frees a scheduler struct
 *
 * @param s
 */
void scheduler_free(struct scheduler *s)
{
    free(s);
}

/**
 * Function passed to pthread_create
 * in order to create a scheduler struct
 * Contains all mutual exclusion required
 *
 * @param ptr scheduler struct
 * @return void*
 */
void* scheduler(void *ptr)
{
    struct scheduler *s = (struct scheduler *)ptr;
    bool finished = false;

#ifdef DEBUG
    pthread_t t = pthread_self();
    D_PRINTF("prod created: %ld\n", t);
#endif

    while(!finished) {
        pthread_mutex_lock(&s->queue->mutex);

        D_PRINTF("prod: q rn: %d\n", s->queue->count);

        while(s->queue->full) {
            // if queue at max capacity, wait for someone to dequeue
            D_PRINTF("prod: q at max: %d\n", s->queue->count);
            pthread_cond_wait(&s->queue->cond_full, &s->queue->mutex);
        }

        request_t *r = file_read_line(s->input);
        if(r) {
            D_PRINTF("prod: added : %d %d\n", r->src, r->dest);
            queue_add(s->queue, r);
            s->total_requests++;
            log_printf(s->logger, "------------------------------\n"
                                  "New Lift Request from %d to %d\n"
                                  "Request No: %d\n"
                                  "------------------------------\n\n", r->src, r->dest, s->total_requests);
            pthread_cond_signal(&s->queue->cond_empty);
        } else {
            // let everyone know we got nothing to put in
            // for anyone waiting for something to be added
            pthread_cond_broadcast(&s->queue->cond_empty);
            s->queue->finished = true;
            finished = true;
        }

        pthread_mutex_unlock(&s->queue->mutex);
    }

    D_PRINTF("prod has died %s\n",  "lol");

    return NULL;
}
