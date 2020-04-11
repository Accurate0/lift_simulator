#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <pthread.h>

#include "queue.h"
#include "log.h"

struct scheduler {
    int total_requests;
    FILE *input;
    struct queue *queue;
    struct logger *logger;
};

struct scheduler* scheduler_init(FILE *input, struct queue *queue, struct logger *logger);
void scheduler_free(struct scheduler *s);
void* scheduler(void *ptr);

#endif
