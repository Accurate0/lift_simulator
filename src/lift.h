#ifndef LIFT_H
#define LIFT_H

#include "queue.h"
#include "mutex.h"

struct lift {
    FILE *output;
    struct queue *queue;
    int lift_time;
};

struct lift* lift_init(FILE *output, struct queue *queue, int lift_time);
void lift_free(struct lift *l);
void *lift(void *ptr);
#endif
