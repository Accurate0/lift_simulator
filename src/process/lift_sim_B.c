// ftruncate
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <common/request.h>
#include <common/file_io.h>
#include <common/debug.h>
#include <common/common.h>

#include "memory.h"
#include "lift_main.h"
#include "scheduler_main.h"

int main(int argc, const char *argv[])
{
    if(argc < 4) {
        fprintf(stderr, "usage: %s [queue size] [time] [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int m = atoi(argv[1]);
    int lift_time = atoi(argv[2]);

    FILE *output = fopen(OUTPUT_FILENAME, "w");
    if(!output) {
        perror("output file");
        return EXIT_FAILURE;
    }

    // Shared Memory
    struct shared_memory *sm = shared_mem_create(m);
    sem_init(&sm->semaphore.empty, 1, m);
    sem_init(&sm->semaphore.full, 1, 0);
    sem_init(&sm->semaphore.mutex, 1, 1);
    sem_init(&sm->semaphore.file, 1, 1);

    pid_t lifts[TOTAL_LIFTS];
    pid_t scheduler;

    for(int i = 0; i < TOTAL_LIFTS; i++) {
        if((lifts[i] = fork()) == 0) {
            // Child
            // Each child will immediately exit main by returning
            return lift_main(i + 1, lift_time, output);
        }
    }

    // Create scheduler
    if((scheduler = fork()) == 0) {
        // Child
        return scheduler_main(argv[3], output);
    }

    // Parent
    for(int i = 0; i < TOTAL_LIFTS; i++) {
        waitpid(lifts[i], NULL, 0);
    }

    waitpid(scheduler, NULL, 0);

    fprintf(output, "Total Number of Requests: %d\n"
                    "Total Number of Movements: %d\n",
                    sm->total_requests, sm->lift_movements[0]
                                        + sm->lift_movements[1]
                                        + sm->lift_movements[2]);

    sem_destroy(&sm->semaphore.mutex);
    sem_destroy(&sm->semaphore.empty);
    sem_destroy(&sm->semaphore.full);
    sem_destroy(&sm->semaphore.file);
    shared_memory_destroy(sm, m);


    fclose(output);

    return 0;
}
