#define _POSIX_C_SOURCE 200112L

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <common/request.h>
#include <common/common.h>

#include "memory.h"

void shared_memory_destroy(struct shared_memory *sm, int m)
{
    munmap(sm->requests, sizeof(request_t) * m);
    munmap(sm, sizeof(struct shared_memory));

    shm_unlink(SHARED_MEMORY_REQUESTS);
    shm_unlink(SHARED_MEMORY_NAME);
}

struct shared_memory* shared_mem_create(int m)
{
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(struct shared_memory));

    struct shared_memory *ptr = mmap(0, sizeof(struct shared_memory), PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int req_list_fd = shm_open(SHARED_MEMORY_REQUESTS, O_CREAT | O_RDWR, 0666);
    ftruncate(req_list_fd, sizeof(request_t) * m);

    ptr->requests = mmap(0, sizeof(request_t) * m, PROT_WRITE, MAP_SHARED, req_list_fd, 0);

    ptr->current = 0;
    ptr->max = m;
    ptr->finished = false;
    ptr->empty = true;
    ptr->full = false;
    ptr->head = -1;
    ptr->tail = -1;
    ptr->total_requests = 0;
    for(int i = 0; i < TOTAL_LIFTS; i++) {
        ptr->lift_movements[i] = 0;
    }
    // struct request r = { .src = 1, .dest = 2 };

    // ptr->requests[0] = r;

    return ptr;
}
