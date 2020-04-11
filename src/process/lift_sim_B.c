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
    (void)argc;
    (void)argv;

    // if(argc < 4) {
    //     fprintf(stderr, "usage: %s [queue size] [time] [filename]\n", argv[0]);
    //     return EXIT_FAILURE;
    // }

    int m = 2;
    // int m = atoi(argv[1]);
    // int lift_time = atoi(argv[2]);

    // FILE *output = fopen(OUTPUT_FILENAME, "w");
    // if(!output) {
    //     perror("output file");
    //     return EXIT_FAILURE;
    // }

    // Shared Memory
    struct shared_memory *sm = shared_mem_create(m);


    pid_t lifts[TOTAL_LIFTS];
    pid_t scheduler;

    for(int i = 0; i < TOTAL_LIFTS; i++) {
        if((lifts[i] = fork()) == 0) {
            // Child
            // Each child will immediately exit main by returning
            return lift_main(i + 1);
        }
    }

    // Create scheduler
    if((scheduler = fork()) == 0) {
        // Child
        return scheduler_main(argv[3]);
    }

    // Parent
    for(int i = 0; i < TOTAL_LIFTS; i++) {
        waitpid(lifts[i], NULL, 0);
    }

    waitpid(scheduler, NULL, 0);

    shared_memory_destroy(sm, m);

    // fclose(output);
    return 0;
}
