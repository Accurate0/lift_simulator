/**
 * @file queue.c
 * @author Anurag Singh (18944183)
 *
 * @date 09-05-20
 *
 * Queue implemenation, backed by a list
 *
 */

#include <stdlib.h>

#include <common/request.h>
#include "queue.h"

/**
 * Initialises a queue struct with
 * a maximum size of m
 *
 * @param m max size
 * @return struct queue*
 */
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
    pthread_cond_init(&queue->cond_empty, NULL);
    pthread_cond_init(&queue->cond_full, NULL);

    return queue;
}

/**
 * Creates a node containing the data
 *
 * @param data data to put in the node
 * @return struct node*
 */
static struct node* node_init(request_t *data)
{
    struct node *node = malloc(sizeof(struct node));
    node->next = NULL;
    node->data = data;

    return node;
}

/**
 * Adds to the end of the list
 * to allow FIFO behaviour
 *
 * @param queue queue to add to
 * @param ptr data to add
 */
void queue_add(struct queue *queue, request_t *ptr)
{
    if(queue->count == queue->max) {
        return;
    }

    struct node *node = node_init(ptr);

    if(queue->tail == NULL) {
        queue->head = node;
        queue->tail = node;
    } else {
        queue->tail->next = node;
        queue->tail = node;
    }

    queue->count++;
    if(queue->count == queue->max) {
        queue->full = true;
    }

    queue->empty = false;
}

/**
 * Removes from the front of the queue
 *
 * @param queue
 * @return request_t*
 */
request_t* queue_remove(struct queue *queue)
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

/**
 * Frees a node
 *
 * @param node
 */
static void node_free(struct node *node)
{
    free(node->data);
    free(node);
}

/**
 * Frees the entire queue including
 * the struct and any mutexes
 *
 * @param queue
 */
void queue_free(struct queue *queue)
{
    struct node *current = queue->head;

    while(current) {
        struct node *next = current->next;

        node_free(current);

        current = next;
    }

    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond_empty);
    pthread_cond_destroy(&queue->cond_full);
    free(queue);
}
