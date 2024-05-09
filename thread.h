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

INLINE u32 thread_create(thread* th, void* (*func)(void*), void* arg) {
    th->func = func;
    th->arg = arg;
#ifdef _WIN32
    th->handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, &th->id);
    return (th->handle != NULL) ? 0 : -1;
#else
    return pthread_create(&th->id, NULL, func, arg);
#endif
}

INLINE u32 thread_join(thread* th) {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(th->handle, INFINITE);
    CloseHandle(th->handle);

    return (ret == WAIT_OBJECT_0) ? 0 : -1;
#else
    return pthread_join(th->id, NULL);
#endif
}

INLINE void thread_sleep(u32 ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}