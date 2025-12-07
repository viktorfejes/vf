/*
*   vf_thread - v0.1
*   Header-only tiny library to help with cross-platform multi-threading.
*
*   RECENT CHANGES:
*       0.1     (2024-08-07)    Finalized the implementation;
*
*   LICENSE: MIT License
*       Copyright (c) 2024 Viktor Fejes
*
*       Permission is hereby granted, free of charge, to any person obtaining a copy
*       of this software and associated documentation files (the "Software"), to deal
*       in the Software without restriction, including without limitation the rights
*       to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*       copies of the Software, and to permit persons to whom the Software is
*       furnished to do so, subject to the following conditions:
*
*       The above copyright notice and this permission notice shall be included in all
*       copies or substantial portions of the Software.
*
*       THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*       IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*       FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*       AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*       LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*       OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*       SOFTWARE.
*
*   TODOs:
*       - [] 
*
 */

#ifndef VF_THREAD_H
#define VF_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef _WIN32
    #ifndef VF_THREAD_NO_WINDOWS_H
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
    #else
        // Minimal Windows types definition
        #ifndef WINAPI
            #define WINAPI __stdcall
        #endif
        typedef void* HANDLE;
        typedef unsigned long DWORD;
        typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE)(void*);
        #define INFINITE 0xFFFFFFFF
        #define WAIT_OBJECT_0 0
        #ifndef BOOL
            typedef int BOOL;
        #endif
        #ifndef TRUE
            #define TRUE 1
        #endif
        #ifndef FALSE
            #define FALSE 0
        #endif
    #endif
#else
    #include <pthread.h>
    #include <unistd.h>
#endif

typedef struct {
#ifdef _WIN32
    HANDLE handle;
    DWORD id;
#else
    pthread_t id;
#endif
    void* (*func)(void*);
    void* arg;
} vf_thread_t;

typedef struct {
#ifdef _WIN32
    CRITICAL_SECTION cs;
#else
    pthread_mutex_t mtx;
#endif
} vf_mutex_t;

typedef struct {
#ifdef _WIN32
    CONDITION_VARIABLE cond;
#else
    pthread_cond_t cond;
#endif
} vf_cond_t;

typedef struct {
#ifdef _WIN32
    SRWLOCK lock;
    LONG readers;
    LONG writer_waiting;
    volatile LONG writer_active;
#else
    pthread_rwlock_t lock;
#endif
} vf_rwlock_t;

typedef struct {
#ifdef _WIN32
    DWORD key;
#else
    pthread_key_t key;
#endif
} vf_tls_key_t;

typedef enum {
    VF_THREAD_SUCCESS = 0,
    VF_THREAD_ERROR_CREATE,
    VF_THREAD_ERROR_JOIN,
    VF_THREAD_ERROR_DETACH,
    VF_THREAD_ERROR_MUTEX_INIT,
    VF_THREAD_ERROR_MUTEX_DESTROY,
    VF_THREAD_ERROR_MUTEX_LOCK,
    VF_THREAD_ERROR_MUTEX_UNLOCK,
    VF_ERROR_TLS_CREATE,
    VF_ERROR_TLS_SET,
    VF_ERROR_TLS_DELETE
} vf_thread_error_t;

/**
 * @brief Spawns a new thread with the supplied function pointer and arguments.
 * 
 * @param thread The thread struct to use.
 * @param func Function pointer to the function.
 * @param arg Arguments for the function pointer.
 * @return thread_error_t Result code.
 */
extern vf_thread_error_t vf_thread_create(vf_thread_t* thread, void* (*func)(void*), void* arg);

/**
 * @brief Waits for the thread and joins it into the main thread.
 * 
 * @param thread The thread to join.
 * @return thread_error_t Result code.
 */
extern vf_thread_error_t vf_thread_join(vf_thread_t* thread);

/**
 * @brief Detaches the thread, allowing it to run independently.
 * 
 * @param thread The thread to detach.
 * @return vf_thread_error_t Result code.
 */
extern vf_thread_error_t vf_thread_detach(vf_thread_t* thread);

/**
 * @brief Sleeps the current thread.
 * 
 * @param ms Duration in milliseconds.
 */
extern void vf_thread_sleep(uint32_t ms);

/**
 * @brief Initializes the selected mutex.
 * 
 * @param mtx The mutex to initialize.
 */
extern void vf_mutex_init(vf_mutex_t* mtx);

/**
 * @brief Destroy the selected mutex.
 * 
 * @param mtx Mutex to destroy.
 */
extern void vf_mutex_destroy(vf_mutex_t* mtx);

/**
 * @brief Locks the mutex.
 * 
 * @param mtx Mutex to lock
 */
extern void vf_mutex_lock(vf_mutex_t* mtx);

/**
 * @brief Unlocks the mutex.
 * 
 * @param mtx Mutex to unlock.
 */
extern void vf_mutex_unlock(vf_mutex_t* mtx);

/**
 * @brief Initializes a condition variable.
 * 
 * @param cond Pointer to the condition variable to initialize.
 */
extern void vf_cond_init(vf_cond_t* cond);

/**
 * @brief Waits on a condition variable.
 * 
 * @param cond Pointer to the condition variable to wait on.
 * @param mutex Pointer to the associated mutex.
 */
extern void vf_cond_wait(vf_cond_t* cond, vf_mutex_t* mutex);

/**
 * @brief Signals a condition variable, waking up one waiting thread.
 * 
 * @param cond Pointer to the condition variable to signal.
 */
extern void vf_cond_signal(vf_cond_t* cond);

/**
 * @brief Broadcasts a condition variable, waking up all waiting threads.
 * 
 * @param cond Pointer to the condition variable to broadcast.
 */
extern void vf_cond_broadcast(vf_cond_t* cond);

/**
 * @brief Destroys a condition variable.
 * 
 * @param cond Pointer to the condition variable to destroy.
 */
extern void vf_cond_destroy(vf_cond_t* cond);

/**
 * @brief Initializes a read-write lock.
 * 
 * @param rwlock Pointer to the read-write lock to initialize.
 */
extern void vf_rwlock_init(vf_rwlock_t* rwlock);

/**
 * @brief Acquires a read lock on a read-write lock.
 * 
 * @param rwlock Pointer to the read-write lock to acquire for reading.
 */
extern void vf_rwlock_rdlock(vf_rwlock_t* rwlock);

/**
 * @brief Acquires a write lock on a read-write lock.
 * 
 * @param rwlock Pointer to the read-write lock to acquire for writing.
 */
extern void vf_rwlock_wrlock(vf_rwlock_t* rwlock);

/**
 * @brief Releases a read-write lock.
 * 
 * @param rwlock Pointer to the read-write lock to release.
 */
extern void vf_rwlock_unlock(vf_rwlock_t* rwlock);

/**
 * @brief Destroys a read-write lock.
 * 
 * @param rwlock Pointer to the read-write lock to destroy.
 */
extern void vf_rwlock_destroy(vf_rwlock_t* rwlock);

/**
 * @brief Creates a thread-local storage key.
 * 
 * @param key Pointer to the TLS key to create.
 * @param destructor Function pointer to the destructor for the TLS data.
 * @return vf_thread_error_t Result code.
 */
extern vf_thread_error_t vf_tls_create(vf_tls_key_t* key, void (*destructor)(void*));

/**
 * @brief Sets a value for a thread-local storage key.
 * 
 * @param key Pointer to the TLS key.
 * @param value Pointer to the value to store.
 * @return vf_thread_error_t Result code.
 */
extern vf_thread_error_t vf_tls_set(vf_tls_key_t* key, const void* value);

/**
 * @brief Retrieves the value associated with a thread-local storage key.
 * 
 * @param key Pointer to the TLS key.
 * @return void* Pointer to the stored value, or NULL if no value was set.
 */
extern void* vf_tls_get(vf_tls_key_t* key);

/**
 * @brief Deletes a thread-local storage key.
 * 
 * @param key Pointer to the TLS key to delete.
 * @return vf_thread_error_t Result code.
 */
extern vf_thread_error_t vf_tls_delete(vf_tls_key_t* key);

// Remove win lean and mean in case it interferes with user's win header include
#undef WIN32_LEAN_AND_MEAN

#ifdef __cplusplus
}
#endif

// END OF HEADER. -----------------------------------------

#ifdef VF_THREAD_IMPLEMENTATION

vf_thread_error_t vf_thread_create(vf_thread_t* thread, void* (*func)(void*), void* arg) {
    thread->func = func;
    thread->arg = arg;
#ifdef _WIN32
    thread->handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, arg, 0, &thread->id);
    return (thread->handle != NULL) ? VF_THREAD_SUCCESS : VF_THREAD_ERROR_CREATE;
#else
    int result = pthread_create(&th->id, NULL, func, arg);
    return (result == 0) ? VF_THREAD_SUCCESS : VF_THREAD_ERROR_CREATE;
#endif
}

vf_thread_error_t vf_thread_join(vf_thread_t* thread) {
#ifdef _WIN32
    DWORD ret = WaitForSingleObject(thread->handle, INFINITE);
    CloseHandle(thread->handle);

    return (ret == WAIT_OBJECT_0) ? VF_THREAD_SUCCESS : VF_THREAD_ERROR_JOIN;
#else
    int result = pthread_join(thread->id, NULL);
    return (result == 0) ? VF_THREAD_SUCCESS : VF_THREAD_ERROR_JOIN;
#endif
}

vf_thread_error_t vf_thread_detach(vf_thread_t* thread) {
#ifdef _WIN32
    if (CloseHandle(thread->handle) != 0) {
        return VF_THREAD_SUCCESS;
    } else {
        return VF_THREAD_ERROR_DETACH;
    }
#else
    int result = pthread_detach(th->id);
    return (result == 0) ? VF_THREAD_SUCCESS : VF_THREAD_ERROR_DETACH;
#endif
}

void vf_thread_sleep(uint32_t ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

void vf_mutex_init(vf_mutex_t* mtx) {
#ifdef _WIN32
    InitializeCriticalSection(&mtx->cs);
#else
    pthread_mutex_init(&mtx->mtx, NULL);
#endif
}

void vf_mutex_destroy(vf_mutex_t* mtx) {
#ifdef _WIN32
    DeleteCriticalSection(&mtx->cs);
#else
    pthread_mutex_destroy(&mtx->mtx);
#endif
}

void vf_mutex_lock(vf_mutex_t* mtx) {
#ifdef _WIN32
    EnterCriticalSection(&mtx->cs);
#else
    pthread_mutex_lock(&mtx->mtx);
#endif
}

void vf_mutex_unlock(vf_mutex_t* mtx) {
#ifdef _WIN32
    LeaveCriticalSection(&mtx->cs);
#else
    pthread_mutex_unlock(&mtx->mtx);
#endif
}

void vf_cond_init(vf_cond_t* cond) {
#ifdef _WIN32
    InitializeConditionVariable(&cond->cond);
#else
    pthread_cond_init(&cond->cond, NULL);
#endif
}

void vf_cond_wait(vf_cond_t* cond, vf_mutex_t* mutex) {
#ifdef _WIN32
    SleepConditionVariableCS(&cond->cond, &mutex->cs, INFINITE);
#else
    pthread_cond_wait(&cond->cond, &mutex->mtx);
#endif
}

void vf_cond_signal(vf_cond_t* cond) {
#ifdef _WIN32
    WakeConditionVariable(&cond->cond);
#else
    pthread_cond_signal(&cond->cond);
#endif
}

void vf_cond_broadcast(vf_cond_t* cond) {
#ifdef _WIN32
    WakeAllConditionVariable(&cond->cond);
#else
    pthread_cond_broadcast(&cond->cond);
#endif
}

void vf_cond_destroy(vf_cond_t* cond) {
#ifdef _WIN32
    // Windows condition variables don't need to be destroyed
#else
    pthread_cond_destroy(&cond->cond);
#endif
}

void vf_rwlock_init(vf_rwlock_t* rwlock) {
#ifdef _WIN32
    InitializeSRWLock(&rwlock->lock);
    rwlock->readers = 0;
    rwlock->writer_waiting = 0;
    rwlock->writer_active = 0;
#else
    pthread_rwlock_init(&rwlock->lock, NULL);
#endif
}

void vf_rwlock_rdlock(vf_rwlock_t* rwlock) {
#ifdef _WIN32
    while (1) {
        AcquireSRWLockShared(&rwlock->lock);
        if (rwlock->writer_waiting == 0) {
            InterlockedIncrement(&rwlock->readers);
            ReleaseSRWLockShared(&rwlock->lock);
            break;
        }
        ReleaseSRWLockShared(&rwlock->lock);
        Sleep(1);  // Small delay before retrying
    }
#else
    pthread_rwlock_rdlock(&rwlock->lock);
#endif
}

void vf_rwlock_wrlock(vf_rwlock_t* rwlock) {
#ifdef _WIN32
    InterlockedIncrement(&rwlock->writer_waiting);
    while (1) {
        if (InterlockedCompareExchange(&rwlock->writer_active, 1, 0) == 0) {
            AcquireSRWLockExclusive(&rwlock->lock);
            InterlockedDecrement(&rwlock->writer_waiting);
            break;
        }
        Sleep(1);  // Small delay before retrying
    }
#else
    pthread_rwlock_wrlock(&rwlock->lock);
#endif
}

void vf_rwlock_unlock(vf_rwlock_t* rwlock) {
#ifdef _WIN32
    if (rwlock->writer_active) {
        InterlockedExchange(&rwlock->writer_active, 0);  // Use InterlockedExchange
        ReleaseSRWLockExclusive(&rwlock->lock);
    } else {
        InterlockedDecrement(&rwlock->readers);
        // No need to release shared lock, it's already released in rdlock
    }
#else
    pthread_rwlock_unlock(&rwlock->lock);
#endif
}

void vf_rwlock_destroy(vf_rwlock_t* rwlock) {
#ifdef _WIN32
    // Windows SRW locks don't need to be destroyed
    rwlock->readers = 0;
    rwlock->writer_waiting = 0;
    rwlock->writer_active = 0;
#else
    pthread_rwlock_destroy(&rwlock->lock);
#endif
}

vf_thread_error_t vf_tls_create(vf_tls_key_t* key, void (*destructor)(void*)) {
#ifdef _WIN32
    key->key = TlsAlloc();
    return (key->key != TLS_OUT_OF_INDEXES) ? VF_THREAD_SUCCESS : VF_ERROR_TLS_CREATE;
#else
    int result = pthread_key_create(&key->key, destructor);
    return (result == 0) ? VF_THREAD_SUCCESS : VF_ERROR_TLS_CREATE;
#endif
}

vf_thread_error_t vf_tls_set(vf_tls_key_t* key, const void* value) {
#ifdef _WIN32
    return TlsSetValue(key->key, (LPVOID)value) ? VF_THREAD_SUCCESS : VF_ERROR_TLS_SET;
#else
    return pthread_setspecific(key->key, value);
#endif
}

void* vf_tls_get(vf_tls_key_t* key) {
#ifdef _WIN32
    return TlsGetValue(key->key);
#else
    return pthread_getspecific(key->key);
#endif
}

vf_thread_error_t vf_tls_delete(vf_tls_key_t* key) {
#ifdef _WIN32
    return TlsFree(key->key) ? VF_THREAD_SUCCESS : VF_ERROR_TLS_DELETE;
#else
    int result = pthread_key_delete(key->key);
    return (result == 0) ? VF_THREAD_SUCCESS : VF_ERROR_TLS_DELETE;
#endif
}

#endif // VF_THREAD_IMPLEMENTATION
#endif // VF_THREAD_H
