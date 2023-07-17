#ifndef _ASYNC_COND_QUEUE_H_
#define _ASYNC_COND_QUEUE_H_

#include "queue.h"
#include <pthread.h>

typedef struct async_queue
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int waiting_threads;
    queue_t * queue;
    int quit;

    long long tasked;
}async_queue_t;

async_queue_t* async_queue_create(int size);

bool async_queue_push_tail(async_queue_t* q,void *data);

void* async_queue_pop_head(async_queue* q,int timeout);

void async_queue_free(async_queue_t* q);

bool async_queue_is_empty(async_queue_t* q);

bool async_queue_wkeup(async_queue_t* q);

#endif