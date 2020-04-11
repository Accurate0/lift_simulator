#ifndef LIFT_H
#define LIFT_H

#include "queue.h"
#include "log.h"

struct lift {
    int id;
    int lift_time;
    int total_movements;
    struct queue *queue;
    struct log *logger;
};

struct lift* lift_init(struct queue *queue, int lift_time, struct log *logger, int id);
void lift_free(struct lift *l);
void *lift(void *ptr);
#endif
