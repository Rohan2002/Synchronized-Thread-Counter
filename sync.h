#pragma once

struct thread_args
{
    int target;
    int counter;
    int delta;
    pthread_mutex_t lock;
    pthread_cond_t cond_producer;
    pthread_cond_t cond_consumer;
};
typedef struct thread_args thread_arg_t;

void *add(void *);
void *sub(void *);