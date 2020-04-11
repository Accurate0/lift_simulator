#ifndef MEMORY_H
#define MEMORY_H

#include <common/request.h>

struct shared_memory {
    request_t *requests;
    int max;
    int current;
};

#define SHARED_MEMORY_NAME          "shared_mem"
#define SHARED_MEMORY_REQUESTS      "requests_list"

void shared_memory_destroy(struct shared_memory *sm, int m);
struct shared_memory* shared_mem_create(int m);

#endif
