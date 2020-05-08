/**
 * @file cqueue.h
 * @author Anurag Singh (18944183)
 *
 * @date 24-04-20
 *
 */

#ifndef CQUEUE_H
#define CQUEUE_H
#include "memory.h"

void sm_cqueue_add(struct shared_memory *sm, request_t r);
request_t sm_cqueue_remove(struct shared_memory *sm);
#endif
