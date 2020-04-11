#include <stdio.h>
#include <stdlib.h>

#include <common/request.h>
#include <common/file_io.h>
#include <common/debug.h>
#include <common/common.h>

#include "queue.h"
#include "scheduler.h"
#include "lift.h"
#include "log.h"

// TODO: Add proccesses after threads is complete

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
    if(!output) {
        perror("output file");
        return EXIT_FAILURE;
    }

    pthread_t threads[TOTAL_THREADS];
    struct lift *lifts[TOTAL_LIFTS];

    struct queue *queue = queue_init(m);
    struct log *logger = log_init(output);

    // thread 0 is task scheduler
    // thread 1 - TOTAL_THREADS are lift threads
    for(int i = 0; i < TOTAL_LIFTS; i++) {
        lifts[i] = lift_init(queue, lift_time, logger, i + 1);
        pthread_create(&threads[i + 1], NULL, lift, lifts[i]);
    }

    struct scheduler *s = scheduler_init(input, queue, logger);
    pthread_create(&threads[0], NULL, scheduler, s);

    // Clean up
    // join all threads
    for(int i = 0; i < TOTAL_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    D_PRINTF("final : %d %d %d %d\n", s->total_requests, lifts[0]->total_movements,
                                                         lifts[1]->total_movements,
                                                         lifts[2]->total_movements);

    // Log the final stuff once the threads are dead
    log_printf(logger, "Total Number of Requests: %d\n"
                       "Total Number of Movements: %d\n",
                       s->total_requests, lifts[0]->total_movements
                                        + lifts[1]->total_movements
                                        + lifts[2]->total_movements);

    for(int i = 0; i < TOTAL_LIFTS; i++) {
        lift_free(lifts[i]);
    }

    fclose(input);
    fclose(output);

    log_free(logger);
    queue_free(queue);
    scheduler_free(s);

    return EXIT_SUCCESS;
}
