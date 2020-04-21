#include <stdio.h>
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
    int previous_floor, current_floor = 0;
    int request_num = 0;
    D_PRINTF("lift %d : created\n", lift_num);
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    int req_fd = shm_open(SHARED_MEMORY_REQUESTS, O_RDWR, 0666);
    struct shared_memory *sm = mmap(0, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sm->requests = mmap(0, sizeof(request_t) * sm->max, PROT_READ | PROT_WRITE, MAP_SHARED, req_fd, 0);

    while(1) {
        // sem_wait(&sm->semaphore.mutex);
        // D_PRINTF("lift %d : empty = %d finished = %d\n", lift_num, sm->empty, sm->finished);
        sem_wait(&sm->semaphore.mutex);
        if(sm->empty && sm->finished) {
            D_PRINTF("lift %d : dead\n", lift_num);
            // sem_post(&sm->semaphore.mutex);
            // sem_post(&sm->semaphore.empty);
            sem_post(&sm->semaphore.full);
            sem_post(&sm->semaphore.mutex);
            break;
        }
        sem_post(&sm->semaphore.mutex);

        // while(sm->empty && !sm->finished) {
        //     D_PRINTF("lift %d : waiting on empty -> %d\n", lift_num, sm->current);
        //     sem_wait(&sm->semaphore.empty);
        // }

        // sem_wait(&sm->semaphore.full);

        if(!sm->empty) {
            D_PRINTF("lift %d : waiting on mutex\n", lift_num);
            sem_wait(&sm->semaphore.full);
            sem_wait(&sm->semaphore.mutex);
            D_PRINTF("lift %d : mutex grabbed\n", lift_num);
            request_t r = sm_cqueue_remove(sm);
            D_PRINTF("lift %d : moving from %d to %d\n", lift_num, r.src, r.dest);
            sem_post(&sm->semaphore.mutex);
            sem_post(&sm->semaphore.empty);

            D_PRINTF("lift %d : mutex released, sleeping for %d\n", lift_num, sleep_time);
            // pretend to work
            sleep(sleep_time);

            sem_wait(&sm->semaphore.file);
            request_num++;
            previous_floor = current_floor;
            current_floor = r.dest;
            int r_movement = abs(previous_floor - r.src) + abs(r.src - current_floor);
            sm->lift_movements[lift_num - 1] += r_movement;

            fprintf(output, "\nLift-%d Operation\n"
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

            sem_post(&sm->semaphore.file);
        }
    }


    fclose(output);
    return 0;
}
