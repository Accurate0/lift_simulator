#ifndef FILE_IO_H
#define FILE_IO_H
#include <stdio.h>
#include <stdbool.h>

#include "request.h"

request_t* file_read_line(FILE *file);
bool file_validate(FILE *file);
#endif
