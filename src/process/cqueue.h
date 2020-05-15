#ifndef CQUEUE_H
#define CQUEUE_H
#include "memory.h"

void sm_cqueue_add(struct shared_memory *sm, request_t r);
request_t sm_cqueue_remove(struct shared_memory *sm);
#endif
