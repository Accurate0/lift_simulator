#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "memory.h"
#include "scheduler_main.h"

int scheduler_main(const char *filename)
{
    (void)filename;
    printf("Scheduler\n");
    int shm_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    struct shared_memory *ptr = mmap(0, sizeof(struct shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    printf("%d\n", ptr->requests[0].dest);

    return 0;
}
