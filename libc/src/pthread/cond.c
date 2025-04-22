#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>

#include "futex.h"

// based on https://doc.rust-lang.org/src/std/sys/sync/condvar/futex.rs.html

int pthread_cond_init(pthread_cond_t *cond, const void *attr) {
    atomic_store(cond, PTHREAD_COND_INITIALIZER);
    return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond) {
    return 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
    return pthread_cond_timedwait(cond, mutex, NULL);
}

// cond = xxx_y
// xxx = counter
// y = 1: signaled, 0: waiting

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
    // Examine the notification counter _before_ we unlock the mutex.
    uint32_t value = atomic_load(cond);

    // Unlock the mutex before going to sleep.
    pthread_mutex_unlock(mutex);

    // Wait, but only if there hasn't been any
    // notification since we unlocked the mutex.
    if (futex_wait_abs(cond, value, abstime) == ETIMEDOUT) {
        return ETIMEDOUT;
    }

    // Lock the mutex again.
    return pthread_mutex_lock(mutex);
}

int pthread_cond_signal(pthread_cond_t *cond) {
    uint32_t old = atomic_fetch_or(cond, 1);
    if ((old & 1) == 0) {
        futex_wake((uint32_t*)cond, 1);
    }
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
    uint32_t old = atomic_fetch_or(cond, 1);
    if ((old & 1) == 0) {
        futex_wake((uint32_t*)cond, INT32_MAX);
    }
    return 0;
}

int pthread_condattr_init(pthread_condattr_t *attr) {
    attr = 0;
    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr) {
    return 0;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id) {
    return 0;
}