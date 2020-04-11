#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <pthread.h>

#include <common/request.h>

struct node {
    struct node *next;
    request_t *data;
};

/// Queue backed by a List
struct queue {
    pthread_mutex_t mutex;
    pthread_cond_t cond_empty;
    pthread_cond_t cond_full;

    struct node *head;
    struct node *tail;

    bool full;
    bool empty;

    bool finished;

    int max;
    int count;
};

struct queue* queue_init(int m);
void queue_add(struct queue *queue, request_t *node);
request_t* queue_remove(struct queue *queue);
void queue_free(struct queue *queue);

#endif
