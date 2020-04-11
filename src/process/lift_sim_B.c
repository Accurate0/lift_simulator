// ftruncate
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <common/request.h>
#include <common/file_io.h>
#include <common/debug.h>
#include <common/common.h>

#include "memory.h"

int lift_main(int lift_num)
{
    printf("Lift : %d\n", lift_num);
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    struct shared_memory *ptr = mmap(0, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    printf("%d\n", ptr->requests[0].dest);

    return 0;
}

int scheduler_main(const char *filename)
{
    (void)filename;
    printf("Scheduler\n");
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    struct shared_memory *ptr = mmap(0, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    printf("%d\n", ptr->requests[0].dest);

    return 0;
}

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
