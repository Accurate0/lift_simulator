/**
 * @file file_io.c
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "file_io.h"
#include "request.h"

/**
 * This function reads a line
 * from the input file, parses it
 * in the format specified, and
 * does a simple sanity check to
 * ensure that its valid, before
 * malloc'ing a struct and returning
 * it containing the values read.
 *
 * @param file to read the line from
 * @return malloc'd request_t* containing the values
 */
request_t* file_read_line(FILE *file)
{
    int src, dest;
    char buffer[BUFSIZ] = {0};

    if(!fgets(buffer, BUFSIZ, file)) {
        return NULL;
    }

    int ret = sscanf(buffer, "%d %d", &src, &dest);
    if(ret != 2) {
        // incase 2 values weren't read from the line
        return NULL;
    }

    // bounds check the input
    if(src > MAX_FLOOR || dest > MAX_FLOOR || src < MIN_FLOOR || dest < MIN_FLOOR) {
        return NULL;
    }

    request_t *req = malloc(sizeof(request_t));
    req->src = src;
    req->dest = dest;

    return req;
}
