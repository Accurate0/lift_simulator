#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#include <common/request.h>
#include <common/debug.h>

#include "lift.h"
#include "queue.h"
#include "log.h"

// #define ABS(n) ((n) < 0 ? -(n) : (n))

struct lift* lift_init(struct queue *queue, int lift_time, struct log *logger, int id)
{
    struct lift *l = malloc(sizeof(struct lift));
    l->total_movements = 0;
    l->logger = logger;
    l->queue = queue;
    l->lift_time = lift_time;
    l->id = id;

    return l;
}

void lift_free(struct lift *l)
{
    free(l);
}

void* lift(void *ptr)
{
    struct lift *l = (struct lift*)ptr;
    // Each lift starts at the first floor
    int current_floor = 1;
    int previous_floor = 1;
    int request_no = 0;

#ifdef DEBUG
    pthread_t t = pthread_self();
    D_PRINTF("consumer created: %ld\n", t);
#endif

    while(1) {
        pthread_mutex_lock(&l->queue->mutex);

        if(l->queue->empty && l->queue->finished) {
            // let everyone know that the queue is empty
            pthread_cond_broadcast(&l->queue->cond_empty);
            pthread_mutex_unlock(&l->queue->mutex);
            break;
        }

        while(l->queue->empty && !l->queue->finished) {
            D_PRINTF("consumer waiting: %ld\n", t);
            pthread_cond_wait(&l->queue->cond_empty, &l->queue->mutex);
        }

        if(!l->queue->empty) {
            D_PRINTF("consumer accessing queue: %ld\n", t);
            request_t *r = queue_remove(l->queue);
            // need to let scheduler thread know that there's space now in the queue
            pthread_cond_signal(&l->queue->cond_full);
            pthread_mutex_unlock(&l->queue->mutex);

            D_PRINTF("consumer working: %d %d sleeping for %d\n", r->src, r->dest, l->lift_time);
            // Simulate work by putting the thread to sleep
            sleep(l->lift_time);

            request_no++;
            previous_floor = current_floor;
            current_floor = r->dest;
            int r_movement = abs(previous_floor - r->src) + abs(r->src - current_floor);
            l->total_movements += r_movement;

            log_printf(l->logger, "Lift-%d Operation\n"
                                  "Previous Floor: %d\n"
                                  "Request: Floor %d to %d\n"
                                  "Details: \n"
                                  "\tGo from Floor %d to %d\n"
                                  "\tGo from Floor %d to %d\n"
                                  "\t# Movements: %d\n"
                                  "\tRequest No: %d\n"
                                  "\tTotal # Movement: %d\n"
                                  "Current Position: Floor %d\n",
                                  l->id, previous_floor, r->src, r->dest,
                                  previous_floor, r->src, r->src, current_floor,
                                  r_movement, request_no, l->total_movements,
                                  current_floor);
            free(r);
        } else {
            pthread_mutex_unlock(&l->queue->mutex);
        }
    }

    D_PRINTF("thread -> %ld has died\n", t);

    return NULL;
}
