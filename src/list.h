#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <pthread.h>

struct node {
    struct node *next;
    void *data;
};

struct queue {
    pthread_mutex_t mutex;
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
void queue_add(struct queue *queue, void *node);
void* queue_remove(struct queue *queue);
void queue_free(struct queue *queue);

#endif
