#include <stdio.h>
#include <stdlib.h>

#include "request.h"
#include "file_io.h"
#include "queue.h"
#include "scheduler.h"
#include "lift.h"
#include "mutex.h"

#define OUTPUT_FILENAME "sim_out"
#define TOTAL_THREADS   4

int main(int argc, const char *argv[])
{
    if(argc < 3) {
        fprintf(stderr, "usage: %s [filename] [time]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int lift_time = atoi(argv[2]);

    FILE *input = fopen(argv[1], "r");
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

    struct queue *queue = queue_init(5);

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
    // free(q_mutex);

    scheduler_free(s);
    lift_free(l);

    return EXIT_SUCCESS;
}
