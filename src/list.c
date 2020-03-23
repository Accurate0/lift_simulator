#include <stdlib.h>

#include "list.h"
// #include "request.h"

list_t* list_init()
{
    list_t *list = malloc(sizeof(list_t));
    list->head = NULL;
    list->tail = NULL;

    return list;
}

node_t* node_init(void *data)
{
    node_t *node = malloc(sizeof(node_t));
    node->next = NULL;
    node->prev = NULL;
    node->data = data;

    return node;
}

void list_add_end(list_t *list, node_t *node)
{
    if(list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }

    // printf("added : %d %d\n", ((request_t*)node->data)->src,((request_t*)node->data)->dest);
}

node_t* list_remove_start(list_t *list)
{
    node_t *ret = list->head;

    if(list->head && list->head->next) {
        list->head = list->head->next;
        list->head->prev = NULL;
    } else if(!list->head->next) {
        list->head = NULL;
    }

    return ret;
}

void node_free(node_t *node)
{
    free(node->data);
    free(node);
}

void list_free(list_t *list)
{
    node_t *current = list->head;

    while(current) {
        node_t *next = current->next;

        node_free(current);

        current = next;
    }

    free(list);
}
