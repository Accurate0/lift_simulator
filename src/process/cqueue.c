/**
 * @file cqueue.c
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 * A simple circular queue "implementation"
 *
 */

#include <common/request.h>

#include "memory.h"

/**
 * Add a request to the queue stored in
 * the shared memory param
 *
 * @param sm shared memory struct
 * @param r request to add to queue
 */
void sm_cqueue_add(struct shared_memory *sm, request_t r)
{
    if(sm->head == -1) {
        sm->head = 0;
        sm->tail = 0;
    } else {
        if(sm->tail == sm->max - 1) {
            sm->tail = 0;
        } else {
            sm->tail++;
        }
    }

    sm->current++;
    if(sm->current == sm->max) {
        sm->full = true;
    }
    sm->empty = false;
    sm->requests[sm->tail] = r;
}

/**
 * Remove a request from the queue
 *
 * @param sm memory to remove from
 * @return request_t
 */
request_t sm_cqueue_remove(struct shared_memory *sm)
{
    request_t r = sm->requests[sm->head];

    if(sm->head == sm->tail) {
        sm->head = -1;
        sm->tail = -1;
    } else {
        if(sm->head == sm->max - 1) {
            sm->head = 0;
        } else {
            sm->head++;
        }
    }

    sm->current--;
    if(sm->current == 0) {
        sm->empty = true;
    }
    sm->full = false;

    return r;
}
