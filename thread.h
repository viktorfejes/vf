/**
 * @file thread.h
 * @author Viktor Fejes (viktor@viktorfejes.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-09
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "defines.h"

#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

typedef struct thread {
#ifdef _WIN32
    HANDLE handle;
    DWORD id;
#else
    pthread_t id;
#endif
    void* (*func)(void*);
    void* arg;
} thread;

typedef struct mutex {
#ifdef _WIN32
    CRITICAL_SECTION cs;
#else
    pthread_mutex_t mtx;
#endif
} mutex;

/**
 * @brief Spawns a new thread with the supplied function pointer and arguments.
 * 
 * @param th The thread struct to use.
 * @param func Function pointer to the function.
 * @param arg Arguments for the function pointer.
 * @return i32 Result code.
 */
INLINE i32 thread_create(thread* th, void* (*func)(void*), void* arg) {
    th->func = func;
    th->arg = arg;
#ifdef _WIN32
    th->handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, &th->id);
    return (th->handle != NULL) ? 0 : -1;
#else
    return pthread_create(&th->id, NULL, func, arg);
#endif
}

/**
 * @brief Waits for the thread and joins it into the main thread.
 * 
 * @param th The thread to join.
 * @return i32 Result code.
 */
INLINE i32 thread_join(thread* th) {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(th->handle, INFINITE);
    CloseHandle(th->handle);

    return (ret == WAIT_OBJECT_0) ? 0 : -1;
#else
    return pthread_join(th->id, NULL);
#endif
}

/**
 * @brief Sleeps the current thread.
 * 
 * @param ms Duration in milliseconds.
 */
INLINE void thread_sleep(u32 ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

/**
 * @brief Initializes the selected mutex.
 * 
 * @param mtx The mutex to initialize.
 */
INLINE void mutex_init(mutex* mtx) {
#ifdef _WIN32
    InitializeCriticalSection(&mtx->cs);
#else
    pthread_mutex_init(&mtx->mtx, NULL);
#endif
}

/**
 * @brief Destroy the selected mutex.
 * 
 * @param mtx Mutex to destroy.
 */
INLINE void mutex_destroy(mutex* mtx) {
#ifdef _WIN32
    DeleteCriticalSection(&mtx->cs);
#else
    pthread_mutex_destroy(&mtx->mtx);
#endif
}

/**
 * @brief Locks the mutex.
 * 
 * @param mtx Mutex to lock
 */
INLINE void mutex_lock(mutex* mtx) {
#ifdef _WIN32
    EnterCriticalSection(&mtx->cs);
#else
    pthread_mutex_lock(&mtx->mtx);
#endif
}

/**
 * @brief Unlocks the mutex.
 * 
 * @param mtx Mutex to unlock.
 */
INLINE void mutex_unlock(mutex* mtx) {
#ifdef _WIN32
    LeaveCriticalSection(&mtx->cs);
#else
    pthread_mutex_unlock(&mtx->mtx);
#endif
}