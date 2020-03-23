#include <stdio.h>
#include <stdlib.h>

#include "request.h"
#include "file_io.h"
#include "list.h"

int main(int argc, const char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "usage: %s [time required by each lift]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int ret = EXIT_SUCCESS;
    list_t *list = read_input(argv[1]);
    if(list) {
        node_t *current = list->head;

        while(current) {
            printf("%d %d\n", ((request_t*)current->data)->src,((request_t*)current->data)->dest);
            // printf("%p\n", current);
            current = current->next;
        }

        list_free(list);
    } else {
        perror("lift_sim");
        ret = EXIT_FAILURE;
    }

    // return EXIT_FAILURE;
    return ret;
}
