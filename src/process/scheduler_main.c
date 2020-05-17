/**
 * @file scheduler_main.c
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 * the function called after fork to create a scheduler child
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <common/file_io.h>

#include "memory.h"
#include "cqueue.h"
#include "scheduler_main.h"

int scheduler_main(const char *filename, FILE *output)
{
    bool finished = false;
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    int req_fd = shm_open(SHARED_MEMORY_REQUESTS, O_RDWR, 0666);
    struct shared_memory *sm = mmap(0, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sm->requests = mmap(0, sizeof(request_t) * sm->max, PROT_READ | PROT_WRITE, MAP_SHARED, req_fd, 0);

    // Make sure we can actually open the input file
    // Else we mark as finished, and post the full mutex
    // to allow each process to wake up and die
    FILE *fp = fopen(filename, "r");
    if(!fp) {
        sem_wait(&sm->semaphore.mutex);
        sm->finished = true;
        sm->empty = true;
        sem_post(&sm->semaphore.mutex);
        sem_post(&sm->semaphore.full);
        perror("input file");
        fclose(output);
        return EXIT_FAILURE;
    }

    while(!finished) {
        request_t *r = file_read_line(fp);
        if(r) {
            sem_wait(&sm->semaphore.empty);
            sem_wait(&sm->semaphore.mutex);
            sm->total_requests++;
            sm_cqueue_add(sm, *r);

            sem_wait(&sm->semaphore.file);
            fprintf(output, "------------------------------\n"
                            "New Lift Request from %d to %d\n"
                            "Request No: %d\n"
                            "------------------------------\n\n", r->src, r->dest, sm->total_requests);
            fflush(output);
            sem_post(&sm->semaphore.file);

            sem_post(&sm->semaphore.mutex);
            sem_post(&sm->semaphore.full);

            free(r);
        } else {
            sm->finished = true;
            finished = true;
            // wake up anything sleeping before i die
            sem_post(&sm->semaphore.full);
        }

    }

    fclose(fp);
    fclose(output);

    return 0;
}
