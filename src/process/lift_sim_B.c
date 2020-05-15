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

    // TODO: Add error checking to fork()
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


    shared_memory_destroy(sm, m);

    fclose(output);

    return 0;
}
