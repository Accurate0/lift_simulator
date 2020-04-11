#include <stdio.h>
#include <stdlib.h>

#include "file_io.h"
#include "request.h"

request_t* file_read_line(FILE *file)
{
    char buffer[BUFSIZ] = {0};
    if(!fgets(buffer, BUFSIZ, file)) {
        return NULL;
    }

    request_t *req = malloc(sizeof(request_t));
    sscanf(buffer, "%d %d", &req->src, &req->dest);

    return req;
}

#if 0
list_t* read_input(const char *filename)
{
    FILE *f = fopen(filename, "r");

    if(!f) {
        return NULL;
    }

    list_t *list = list_init();
    // BUFSIZ from libc
    char buffer[BUFSIZ] = {0};
    // int src, dest;

    while(!feof(f)) {
        if(fgets(buffer, BUFSIZ, f)) {
            request_t *req = malloc(sizeof(request_t));

            sscanf(buffer, "%d %d", &req->src, &req->dest);

            list_add_end(list, node_init(req));
        }
    }

    fclose(f);

    return list;
}
#endif
