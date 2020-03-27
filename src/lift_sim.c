#include <stdio.h>
#include <stdlib.h>

#include "request.h"
#include "file_io.h"
#include "queue.h"
#include "scheduler.h"
#include "lift.h"
#include "log.h"
#include "debug.h"

#define OUTPUT_FILENAME         "sim_out"
#define TOTAL_THREADS           4
#define TOTAL_LIFTS             3

// TODO: Add proccesses after threads is complete
// TODO: Probably use #ifdef for that, not reason to
// TODO: split into another file

// TODO: Report

// TODO: Commenting

int main(int argc, const char *argv[])
{
    if(argc < 4) {
        fprintf(stderr, "usage: %s [queue size] [time] [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int m = atoi(argv[1]);
    int lift_time = atoi(argv[2]);

    FILE *input = fopen(argv[3], "r");
    if(!input) {
        perror("input file");
        return EXIT_FAILURE;
    }

    FILE *output = fopen(OUTPUT_FILENAME, "w");
    if(!input) {
        perror("output file");
        return EXIT_FAILURE;
    }

    pthread_t threads[TOTAL_THREADS];
    struct lift *lifts[TOTAL_LIFTS];
    int *total_ptrs[TOTAL_THREADS];

    struct queue *queue = queue_init(m);
    struct logger *logger = logger_init(output);

    // thread 0 is task scheduler
    // thread 1 - TOTAL_THREADS are lift threads
    for(int i = 0; i < TOTAL_LIFTS; i++) {
        lifts[i] = lift_init(queue, lift_time, logger, i + 1);
        pthread_create(&threads[i + 1], NULL, lift, lifts[i]);
    }

    struct scheduler *s = scheduler_init(input, queue, logger);
    pthread_create(&threads[0], NULL, scheduler, s);

    // Contains all the returned int pointers
    // thread 0 returns total requests
    // thread 1-3 returns each of their total movements

    // Clean up
    // join all threads
    for(int i = 0; i < TOTAL_THREADS; i++) {
        pthread_join(threads[i], (void**)&total_ptrs[i]);
    }

    D_PRINTF("final : %d %d %d %d\n", *total_ptrs[0], *total_ptrs[1], *total_ptrs[2], *total_ptrs[3]);

    // Log the final stuff once the threads are dead
    FILE_LOG(logger, "Total Number of Requests: %d\n"
                     "Total Number of Movements: %d\n",
                     *total_ptrs[0], (*total_ptrs[1] + *total_ptrs[2] + *total_ptrs[3]));

    for(int i = 0; i < TOTAL_THREADS; i++) {
        free(total_ptrs[i]);
    }

    for(int i = 0; i < TOTAL_LIFTS; i++) {
        lift_free(lifts[i]);
    }

    fclose(input);
    fclose(output);

    logger_free(logger);
    queue_free(queue);
    scheduler_free(s);

    return EXIT_SUCCESS;
}
