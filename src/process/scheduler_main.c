#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <common/file_io.h>
#include <common/debug.h>

#include "memory.h"
#include "cqueue.h"
#include "scheduler_main.h"

int scheduler_main(const char *filename, FILE *output)
{
    FILE *fp = fopen(filename, "r");
    bool finished = false;
    D_PRINTF("sched %d : created\n", getpid());
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    int req_fd = shm_open(SHARED_MEMORY_REQUESTS, O_RDWR, 0666);
    struct shared_memory *sm = mmap(0, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sm->requests = mmap(0, sizeof(request_t) * sm->max, PROT_READ | PROT_WRITE, MAP_SHARED, req_fd, 0);

    while(!finished) {
        // D_PRINTF("sched: mutex\n", "");
        // sem_wait(&sm->semaphore.mutex);
        // while(sm->full) {
        //     D_PRINTF("sched : waiting due to full -> %d\n", sm->current);
        //     sem_wait(&sm->semaphore.full);
        //     D_PRINTF("sched %d : woken up\n", getpid());
        // }
        // sem_post(&sm->semaphore.mutex);

        request_t *r = file_read_line(fp);
        if(r) {
            D_PRINTF("sched %d : waiting on mutex\n", getpid());
            sem_wait(&sm->semaphore.empty);
            sem_wait(&sm->semaphore.mutex);
            D_PRINTF("sched %d : mutex grabbed\n", getpid());
            D_PRINTF("sched : read: %d %d\n", r->src, r->dest);
            sm->total_requests++;
            sm_cqueue_add(sm, *r);
            sem_post(&sm->semaphore.mutex);
            sem_post(&sm->semaphore.full);

            sem_wait(&sm->semaphore.file);
            fprintf(output, "------------------------------\n"
                            "New Lift Request from %d to %d\n"
                            "Request No: %d\n"
                            "------------------------------\n", r->src, r->dest, sm->total_requests);
            sem_post(&sm->semaphore.file);

            D_PRINTF("sched %d : mutex released\n", getpid());

            free(r);
        } else {
            sm->finished = true;
            finished = true;
            // wake up anything sleeping before i die
            // sem_post(&sm->semaphore.mutex);
            sem_post(&sm->semaphore.full);
            // sem_post(&sm->semaphore.empty);
        }

    }

    D_PRINTF("sched %d : dead\n", getpid());
    fclose(fp);
    fclose(output);

    return 0;
}
