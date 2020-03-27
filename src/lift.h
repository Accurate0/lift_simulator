#ifndef LIFT_H
#define LIFT_H

#include "queue.h"
#include "log.h"

struct lift {
    int id;
    int lift_time;
    struct queue *queue;
    struct logger *logger;
};

struct lift* lift_init(struct queue *queue, int lift_time, struct logger *logger, int id);
void lift_free(struct lift *l);
void *lift(void *ptr);
#endif
