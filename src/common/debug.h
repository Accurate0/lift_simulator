/**
 * @file debug.h
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 */

#ifndef DEBUG_H
#define DEBUG_H

/// Allows debugging print statements that can be easily hidden
#ifdef DEBUG
    #define D_PRINTF(s, ...) fprintf(stderr, s, __VA_ARGS__)
#else
    #define D_PRINTF(s, ...)
#endif

#endif
