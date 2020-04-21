#ifndef MEMORY_H
#define MEMORY_H
#include <stdbool.h>
#include <semaphore.h>

#include <common/request.h>
#include <common/common.h>

struct shared_memory {
    request_t *requests;
    int max;
    int current;
    int head;
    int tail;

    int total_requests;
    int lift_movements[TOTAL_LIFTS];

    bool finished;
    bool empty;
    bool full;

    struct semaphore {
        sem_t full;
        sem_t empty;
        sem_t mutex;
        sem_t file;
    } semaphore;
};

#define SHARED_MEMORY_NAME          "shared_mem"
#define SHARED_MEMORY_REQUESTS      "requests_list"

void shared_memory_destroy(struct shared_memory *sm, int m);
struct shared_memory* shared_mem_create(int m);

#endif
