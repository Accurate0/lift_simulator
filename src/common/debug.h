#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
    #define D_PRINTF(s, ...) fprintf(stderr, s, __VA_ARGS__)
#else
    #define D_PRINTF(s, ...)
#endif

#endif
