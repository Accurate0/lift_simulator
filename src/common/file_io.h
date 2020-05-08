/**
 * @file file_io.h
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 */

#ifndef FILE_IO_H
#define FILE_IO_H
#include <stdio.h>

#include "request.h"

request_t* file_read_line(FILE *file);
#endif
