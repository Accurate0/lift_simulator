/**
 * @file lift_sim_B.c
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 * The actual main of the process part
 * Loads up shared memory, and setups up
 * to fork and create child processes,
 * while also waiting for them to
 * die so we can clean up afterwards
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <common/request.h>
#include <common/file_io.h>
#include <common/common.h>

#include "memory.h"
#include "lift_main.h"
#include "scheduler_main.h"

int main(int argc, const char *argv[])
{
    if(argc < 4) {
        fprintf(stderr, "usage: %s [buffer size] [time] [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int m = atoi(argv[1]);
    int lift_time = atoi(argv[2]);

    if(m <= 0 && lift_time < 0) {
        fprintf(stderr, "buffer size > 0, time >= 0\n");
        return EXIT_FAILURE;
    }

    FILE *output = fopen(OUTPUT_FILENAME, "w");
    if(!output) {
        perror("output file");
        return EXIT_FAILURE;
    }

    FILE *input = fopen(argv[3], "r");
    if(!input) {
        perror("input file");
        return EXIT_FAILURE;
    }

    bool valid = file_validate(input);
    fclose(input);

    if(!valid) {
        fclose(output);

        fprintf(stderr, "invalid input file: %s\n", argv[3]);
        return EXIT_FAILURE;
    }

    // Shared Memory
    struct shared_memory *sm = shared_mem_create(m);

    pid_t lifts[TOTAL_LIFTS];
    pid_t scheduler;
    int total_children = 0;

    // TODO: Add error checking to fork()
    for(int i = 0; i < TOTAL_LIFTS; i++) {
        if((lifts[i] = fork()) == 0) {
            // Child
            // Each child will immediately exit main by returning
            total_children++;
            return lift_main(i + 1, lift_time, output);
        } else if(lifts[i] < 0) {
            // Signal to end all processes that might have started
            sem_wait(&sm->semaphore.mutex);
            sm->empty = true;
            sm->finished = true;
            sem_post(&sm->semaphore.mutex);

            // Wait to prevent zombies
            for(int j = 0; j < total_children; j++) {
                waitpid(lifts[j], NULL, 0);
            }

            perror("fork");
            // Clean up
            fclose(output);
            shared_memory_destroy(sm, m);
            return EXIT_FAILURE;
        }
    }

    // Create scheduler
    if((scheduler = fork()) == 0) {
        // Child
        return scheduler_main(argv[3], output);
    } else if(scheduler < 0) {
        sem_wait(&sm->semaphore.mutex);
        sm->empty = true;
        sm->finished = true;
        sem_post(&sm->semaphore.mutex);
    }

    // Parent
    for(int i = 0; i < TOTAL_LIFTS; i++) {
        waitpid(lifts[i], NULL, 0);
    }

    // If the scheduler fork worked
    if(scheduler > 0) {
        waitpid(scheduler, NULL, 0);

        fprintf(output, "Total Number of Requests: %d\n"
                        "Total Number of Movements: %d\n",
                        sm->total_requests, sm->lift_movements[0]
                                            + sm->lift_movements[1]
                                            + sm->lift_movements[2]);

    }

    fclose(output);
    shared_memory_destroy(sm, m);

    return 0;
}
