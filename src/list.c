#include <stdlib.h>

#include "list.h"
// #include "request.h"

struct queue* queue_init(int m)
{
    struct queue *queue = malloc(sizeof(struct queue));
    queue->max = m;
    queue->count = 0;
    queue->head = NULL;
    queue->tail = NULL;
    queue->empty = true;
    queue->full = false;
    queue->finished = false;

    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_full, NULL);

    return queue;
}

static struct node* node_init(void *data)
{
    struct node *node = malloc(sizeof(struct node));
    node->next = NULL;
    node->data = data;

    return node;
}

void queue_add(struct queue *queue, void *ptr)
{
    struct node *node = node_init(ptr);
    if(queue->count == queue->max) {
        return;
    }

    if(queue->tail == NULL) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
        queue->empty = false;
    }

    queue->count++;
    if(queue->count == queue->max) {
        queue->full = true;
    }


    // printf("added : %d %d\n", ((request_t*)node->data)->src,((request_t*)node->data)->dest);
}

void* queue_remove(struct queue *queue)
{
    struct node *node = queue->head;

    if(!node) {
        return NULL;
    }

    void *data = queue->head->data;

    if(queue->head && queue->head->next) {
        queue->head = queue->head->next;
    } else if(!queue->head->next) {
        queue->head = NULL;
        queue->tail = NULL;
        queue->empty = true;
    }

    queue->full = false;
    queue->count--;
    free(node);
    return data;
}

static void node_free(struct node *node)
{
    free(node->data);
    free(node);
}

void queue_free(struct queue *queue)
{
    struct node *current = queue->head;

    while(current) {
        struct node *next = current->next;

        node_free(current);

        current = next;
    }

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond_full);
    free(queue);
}
