#ifndef DEBUG_PRINTF_H
#define DEBUG_PRINTF_H

#ifdef DEBUG
    #define D_PRINTF(s, ...) fprintf(stderr, s, __VA_ARGS__)
#else
    #define D_PRINTF(s, ...)
#endif

#endif
