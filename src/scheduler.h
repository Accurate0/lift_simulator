#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <pthread.h>

#include "queue.h"
#include "mutex.h"

struct scheduler {
    FILE *input;
    FILE *output;
    struct queue *queue;
};

struct scheduler* scheduler_init(FILE *input, FILE *output, struct queue *queue);
void scheduler_free(struct scheduler *s);
void* scheduler(void *ptr);

#endif
