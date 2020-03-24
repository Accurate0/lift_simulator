#include <stdlib.h>

#include "scheduler.h"
#include "file_io.h"
#include "request.h"
#include "debug.h"

// This is the scheduler thread
// Gets tasks from the queue/list
// and put them into a "buffer"
// read by the lift threads
struct scheduler* scheduler_init(FILE *input, FILE *output, struct queue *queue)
{
    struct scheduler *s = malloc(sizeof(struct scheduler));
    s->input = input;
    s->output = output;
    s->queue = queue;

    return s;
}

void scheduler_free(struct scheduler *s)
{
    free(s);
}

void* scheduler(void *ptr)
{
    struct scheduler *s = (struct scheduler *)ptr;
    int request_num = 0;
    (void)request_num;

    bool finished = false;

    while(!finished) {
        pthread_mutex_lock(&s->queue->mutex);

        // if queue at max capacity, wait for someone to dequeue

        d_printf("q rn: %d\n", s->queue->count);
        while(s->queue->full) {
            d_printf("q at max: %d\n", s->queue->count);
            pthread_cond_wait(&s->queue->cond_full, &s->queue->mutex);
        }

        request_t *r = file_read_line(s->input);
        if(r) {
            queue_add(s->queue, r);
            pthread_cond_signal(&s->queue->cond_full);
        } else {
            pthread_cond_broadcast(&s->queue->cond_full);
            s->queue->finished = true;
            finished = true;
        }

        pthread_mutex_unlock(&s->queue->mutex);
    }

    return NULL;
}
