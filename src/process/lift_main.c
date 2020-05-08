/**
 * @file lift_main.c
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 * The function called when a lift process needs
 * to be created. Effectively the "main" of the lift.
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

#include <common/debug.h>

#include "memory.h"
#include "cqueue.h"
#include "lift_main.h"

int lift_main(int lift_num, int sleep_time, FILE *output)
{
    int previous_floor = 1;
    int current_floor = 1;
    int request_num = 0;
    D_PRINTF("lift %d : created\n", lift_num);
    // Get access to the shared memory
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    int req_fd = shm_open(SHARED_MEMORY_REQUESTS, O_RDWR, 0666);
    struct shared_memory *sm = mmap(0, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sm->requests = mmap(0, sizeof(request_t) * sm->max, PROT_READ | PROT_WRITE, MAP_SHARED, req_fd, 0);

    while(1) {
        sem_wait(&sm->semaphore.mutex);
        if(sm->empty && sm->finished) {
            D_PRINTF("lift %d : dead\n", lift_num);
            sem_post(&sm->semaphore.full);
            sem_post(&sm->semaphore.mutex);
            break;
        }
        sem_post(&sm->semaphore.mutex);
        // we release the mutex after checking
        // if the process should die, because
        // to solve the bounded buffer problem
        // we need to wait on the full semaphore
        // which will be posted by the producer
        // waiting while keeping holding the
        // "mutex" will prevent progress, and likely deadlock


        D_PRINTF("lift %d : waiting on mutex\n", lift_num);
        sem_wait(&sm->semaphore.full);
        sem_wait(&sm->semaphore.mutex);
        D_PRINTF("lift %d : mutex grabbed\n", lift_num);

        if(!sm->empty) {
            request_t r = sm_cqueue_remove(sm);
            D_PRINTF("lift %d : moving from %d to %d\n", lift_num, r.src, r.dest);

            sem_wait(&sm->semaphore.file);
            request_num++;
            previous_floor = current_floor;
            current_floor = r.dest;
            int r_movement = abs(previous_floor - r.src) + abs(r.src - current_floor);
            sm->lift_movements[lift_num - 1] += r_movement;

            fprintf(output, "Lift-%d Operation\n"
                            "Previous Floor: %d\n"
                            "Request: Floor %d to %d\n"
                            "Details: \n"
                            "\tGo from Floor %d to %d\n"
                            "\tGo from Floor %d to %d\n"
                            "\t# Movements: %d\n"
                            "\tRequest No: %d\n"
                            "\tTotal # Movement: %d\n"
                            "Current Position: Floor %d\n\n",
                            lift_num, previous_floor, r.src, r.dest,
                            previous_floor, r.src, r.src, current_floor,
                            r_movement, request_num, sm->lift_movements[lift_num - 1],
                            current_floor);
            fflush(output);

            sem_post(&sm->semaphore.file);
            sem_post(&sm->semaphore.mutex);
            sem_post(&sm->semaphore.empty);
            D_PRINTF("lift %d : mutex released\n", lift_num);

            // release the mutex since the critical section
            // doesn't involve the work we do (in this case)
            // pretend to work
            sleep(sleep_time);
        } else {
            sem_post(&sm->semaphore.mutex);
            sem_post(&sm->semaphore.empty);
        }
    }


    fclose(output);
    return 0;
}
