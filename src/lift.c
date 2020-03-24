#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include "lift.h"
#include "queue.h"
#include "request.h"
#include "mutex.h"
#include "debug.h"

struct lift* lift_init(FILE *output, struct queue *queue, int lift_time)
{
    struct lift *l = malloc(sizeof(struct lift));
    l->output = output;
    l->queue = queue;
    l->lift_time = lift_time;

    return l;
}

void lift_free(struct lift *l)
{
    free(l);
}

void* lift(void *ptr)
{
    struct lift *l = (struct lift*)ptr;

    pthread_t t = pthread_self();
    // printf("created thread -> %ld\n", t);

    while(1) {
        pthread_mutex_lock(&l->queue->mutex);

        if(l->queue->finished) {
            if(l->queue->empty) {
                pthread_cond_broadcast(&l->queue->cond_full);
                pthread_mutex_unlock(&l->queue->mutex);
                break;
            }
        }

        while(l->queue->empty && !l->queue->finished) {
            pthread_cond_wait(&l->queue->cond_full, &l->queue->mutex);
        }

        if(!l->queue->empty) {
            d_printf("thread -> %ld\n", t);
            request_t *r = queue_remove(l->queue);
            // need to let scheduler thread know that there's space now in the queue
            pthread_cond_signal(&l->queue->cond_full);
            // unlock and then sleep

            if(r) {
                d_printf("%d %d sleeping for %d\n", r->src, r->dest, l->lift_time);
                free(r);
                pthread_mutex_unlock(&l->queue->mutex);
                // simulate lift
                // gotta unlock so the queue can be used
                // by other threads
                sleep(l->lift_time);
            }
        }
    }

    return NULL;
}
