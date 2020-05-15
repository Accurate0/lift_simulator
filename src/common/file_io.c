/**
 * @file file_io.c
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

/**
 * Validates that the input file has
 * only valid lift locations and
 * has a valid size between the min and
 * max inclusive
 *
 * @param file to be validated
 * @return true if the file is valid
 * @return false if the file is invalid
 */
bool file_validate(FILE *file)
{
    request_t *req;
    int count = 0;

    while((req = file_read_line(file))) {
        count++;
        free(req);
    }

    // If we didn't reach eof
    // it means there was an invalid line
    if(!feof(file)) {
        return false;
    }

    // If the overall count is less than or more than
    // its also invalid
    if(count > MAX_INPUT || count < MIN_INPUT) {
        return false;
    }

    rewind(file);

    return true;
}
