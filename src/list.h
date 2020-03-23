#ifndef LIST_H
#define LIST_H

typedef struct node_t {
    struct node_t *next;
    struct node_t *prev;
    void *data;
} node_t;

typedef struct {
    node_t *head;
    node_t *tail;
} list_t;

list_t* list_init();
node_t* node_init(void *data);
void list_add_end(list_t *list, node_t *node);
node_t* list_remove_start(list_t *list);
void node_free(node_t *node);
void list_free(list_t *list);

#endif
