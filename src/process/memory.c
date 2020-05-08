/**
 * @file memory.c
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 * Handles allocating and freeing shared memory
 *
 */

// ftruncate
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

/**
 * Create the shared memory
 * initialising all fields to sane defaults
 * Uses the POSIX shared memory API
 *
 * @param m buffer size
 * @return struct shared_memory*
 */
struct shared_memory* shared_mem_create(int m)
{
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(struct shared_memory));
    struct shared_memory *ptr = mmap(0, sizeof(struct shared_memory), PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int req_list_fd = shm_open(SHARED_MEMORY_REQUESTS, O_CREAT | O_RDWR, 0666);
    ftruncate(req_list_fd, sizeof(request_t) * m);
    ptr->requests = mmap(0, sizeof(request_t) * m, PROT_WRITE, MAP_SHARED, req_list_fd, 0);

    // i think the memory is zero'd out
    // but just incase, we need sane defaults
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

    sem_init(&ptr->semaphore.empty, 1, m);
    sem_init(&ptr->semaphore.full, 1, 0);
    sem_init(&ptr->semaphore.mutex, 1, 1);
    sem_init(&ptr->semaphore.file, 1, 1);

    return ptr;
}

/**
 * Unlinks and unmaps all of the shared memory
 *
 * @param sm shared memory block
 * @param m buffer size (munmap needs this)
 */
void shared_memory_destroy(struct shared_memory *sm, int m)
{
    sem_destroy(&sm->semaphore.mutex);
    sem_destroy(&sm->semaphore.empty);
    sem_destroy(&sm->semaphore.full);
    sem_destroy(&sm->semaphore.file);

    munmap(sm->requests, sizeof(request_t) * m);
    munmap(sm, sizeof(struct shared_memory));

    shm_unlink(SHARED_MEMORY_REQUESTS);
    shm_unlink(SHARED_MEMORY_NAME);
}
