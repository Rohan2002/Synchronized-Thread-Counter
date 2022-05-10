#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "sync.h"

void *add(void *arg)
{
    thread_arg_t *thread_args = arg;
    while (1)
    {
        pthread_mutex_lock(&thread_args->lock);
        // exceeds our target. so block producer and allow consumer to reduce total.
        while (thread_args->counter > thread_args->target)
        {
            pthread_cond_wait(&thread_args->cond_producer, &thread_args->lock);
        }
        // found target found.
        if (thread_args->counter == thread_args->target)
        {
            // wake up all remaining consumer threads, so that they can release their lock safely and exit.
            pthread_cond_broadcast(&thread_args->cond_consumer);
            pthread_mutex_unlock(&thread_args->lock);
            break;
        }
        else
        {
            thread_args->counter += thread_args->delta;
            printf("Increment: %d\n", thread_args->counter);
        }
        pthread_mutex_unlock(&thread_args->lock);
    }
    return NULL;
}
void *sub(void *arg)
{
    thread_arg_t *thread_args = arg;
    while (1)
    {
        pthread_mutex_lock(&thread_args->lock);
        // consumer decreased the value too much. Block consumer until producer increments the value.
        while (thread_args->counter < 0)
        {
            pthread_cond_wait(&thread_args->cond_consumer, &thread_args->lock);
        }
        if (thread_args->counter == thread_args->target)
        {
            // wake up all remaining producer threads, so that they can release their lock safely and exit.
            pthread_cond_broadcast(&thread_args->cond_producer);
            pthread_mutex_unlock(&thread_args->lock);
            break;
        }
        else
        {
            thread_args->counter -= thread_args->delta;
            printf("Decrement: %d\n", thread_args->counter);
        }
        pthread_mutex_unlock(&thread_args->lock);
    }
    return NULL;
}
int main()
{
    int producers = 1;
    int consumers = 1;

    pthread_t *producer_ts = malloc(producers * sizeof(pthread_t));
    pthread_t *consumer_ts = malloc(consumers * sizeof(pthread_t));

    thread_arg_t *thread_arg_obj = malloc(sizeof(thread_arg_t));

    thread_arg_obj->target = 100000;
    thread_arg_obj->counter = 0;
    thread_arg_obj->delta = 10;

    pthread_mutex_init(&thread_arg_obj->lock, NULL);
    pthread_cond_init(&thread_arg_obj->cond_producer, NULL);
    pthread_cond_init(&thread_arg_obj->cond_consumer, NULL);

    int i = 0;
    int j = 0;
    for (; i < producers; i++)
    {
        pthread_create(&producer_ts[i], NULL, add, thread_arg_obj);
    }
    for (; j < consumers; j++)
    {
        pthread_create(&consumer_ts[j], NULL, sub, thread_arg_obj);
    }
    int k = 0;
    int l = 0;

    for (; k < producers; k++)
    {
        pthread_join(producer_ts[k], NULL);
    }
    for (; l < consumers; l++)
    {
        pthread_join(consumer_ts[l], NULL);
    }
    printf("Counter: %d and Target: %d\n", thread_arg_obj->counter, thread_arg_obj->target);

    free(producer_ts);
    free(consumer_ts);
    free(thread_arg_obj);
}