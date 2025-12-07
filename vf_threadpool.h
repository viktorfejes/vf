#pragma once

#include <stdlib.h>

#define MAX_THREADS 32
#define MAX_QUEUE 256

typedef struct {
    void (*function)(void *);
    void *argument;
} vf_task_t;

typedef struct {
    vf_thread_t threads[MAX_THREADS];
    vf_task_t queue[MAX_QUEUE];
    int thread_count;
    int queue_size;
    int front;
    int rear;
    vf_mutex_t queue_mutex;
    vf_cond_t queue_not_empty;
    vf_cond_t queue_not_full;
    int stop;
} vf_threadpool_t;

void* threadpool_worker(void* arg) {
    vf_threadpool_t* pool = (vf_threadpool_t*)arg;
    vf_task_t task;

    while (1) {
        vf_mutex_lock(&pool->queue_mutex);

        while (pool->queue_size == 0 && !pool->stop) {
            vf_cond_wait(&pool->queue_not_empty, &pool->queue_mutex);
        }

        if (pool->stop) {
            vf_mutex_unlock(&pool->queue_mutex);
            return NULL;
        }

        task = pool->queue[pool->front];
        pool->front = (pool->front + 1) % MAX_QUEUE;
        pool->queue_size--;

        vf_cond_signal(&pool->queue_not_full);
        vf_mutex_unlock(&pool->queue_mutex);

        (*(task.function))(task.argument);
    }

    return NULL;
}

vf_threadpool_t* vf_threadpool_create(int num_threads) {
    if (num_threads <= 0 || num_threads > MAX_THREADS) {
        return NULL;
    }

    vf_threadpool_t* pool = (vf_threadpool_t*)malloc(sizeof(vf_threadpool_t));
    if (pool == NULL) {
        return NULL;
    }

    pool->thread_count = num_threads;
    pool->queue_size = 0;
    pool->front = 0;
    pool->rear = 0;
    pool->stop = 0;

    vf_mutex_init(&pool->queue_mutex);
    vf_cond_init(&pool->queue_not_empty);
    vf_cond_init(&pool->queue_not_full);

    for (int i = 0; i < num_threads; i++) {
        if (vf_thread_create(&pool->threads[i], threadpool_worker, pool) != VF_THREAD_SUCCESS) {
            vf_threadpool_destroy(pool);
            return NULL;
        }
    }

    return pool;
}

vf_thread_error_t vf_threadpool_add_task(vf_threadpool_t* pool, void (*function)(void*), void* argument) {
    vf_mutex_lock(&pool->queue_mutex);

    while (pool->queue_size == MAX_QUEUE && !pool->stop) {
        vf_cond_wait(&pool->queue_not_full, &pool->queue_mutex);
    }

    if (pool->stop) {
        vf_mutex_unlock(&pool->queue_mutex);
        return VF_THREAD_ERROR_THREADPOOL_STOPPED;
    }

    pool->queue[pool->rear].function = function;
    pool->queue[pool->rear].argument = argument;
    pool->rear = (pool->rear + 1) % MAX_QUEUE;
    pool->queue_size++;

    vf_cond_signal(&pool->queue_not_empty);
    vf_mutex_unlock(&pool->queue_mutex);

    return VF_THREAD_SUCCESS;
}

void vf_threadpool_destroy(vf_threadpool_t* pool) {
    if (pool == NULL) {
        return;
    }

    vf_mutex_lock(&pool->queue_mutex);
    pool->stop = 1;
    vf_cond_broadcast(&pool->queue_not_empty);
    vf_cond_broadcast(&pool->queue_not_full);
    vf_mutex_unlock(&pool->queue_mutex);

    for (int i = 0; i < pool->thread_count; i++) {
        vf_thread_join(&pool->threads[i]);
    }

    vf_mutex_destroy(&pool->queue_mutex);
    vf_cond_destroy(&pool->queue_not_empty);
    vf_cond_destroy(&pool->queue_not_full);

    free(pool);
}