#include <stdio.h>
#include <stdlib.h>

#include "request.h"
#include "file_io.h"
#include "queue.h"
#include "scheduler.h"
#include "lift.h"
#include "mutex.h"

#define OUTPUT_FILENAME         "sim_out"
#define TOTAL_THREADS           4

// TODO: Need to add logging along with a struct + mutex
// TODO: Keep it similar to how the queue struct is

// TODO: Track current position for each elevator

// TODO: Add some shared statistic tracking

// TODO: Fix test script, figure out why it freezes randomly

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

    int m =  atoi(argv[1]);
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

    struct queue *queue = queue_init(m);

    // thread 0 is task scheduler
    // thread 1 - TOTAL_THREADS are lift threads

    struct lift *l = lift_init(output, queue, lift_time);
    // create all lift threads
    for(int i = 1; i < TOTAL_THREADS; i++) {
        pthread_create(&threads[i], NULL, lift, l);
    }

    struct scheduler *s = scheduler_init(input, output, queue);
    pthread_create(&threads[0], NULL, scheduler, s);
    // Clean up
    // join all threads
    for(int i = 0; i < TOTAL_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    fclose(input);
    fclose(output);

    queue_free(queue);

    scheduler_free(s);
    lift_free(l);

    return EXIT_SUCCESS;
}
