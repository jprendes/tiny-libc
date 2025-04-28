#pragma once

#include <stdint.h>
#include <errno.h>
#include <time.h>

// pthread_create / pthread_join / pthread_attr_* are used in in quickjs-ng's cutils.c.
// However, they are only used from quickjs-libc.c, so it gets optimized away.
#define pthread_t int
#define pthread_attr_t int
#define PTHREAD_CREATE_DETACHED 1
static inline int pthread_attr_init(pthread_attr_t *attr) {
    *attr = 0;
    return 0;
}
static inline int pthread_attr_destroy(pthread_attr_t *attr) {
    (void)attr;
    return 0;
}
static inline int pthread_attr_setstacksize(pthread_attr_t *attr, long size) {
    (void)attr;
    (void)size;
    return 0;
}
static inline int pthread_attr_setdetachstate(pthread_attr_t *attr, int state) {
    (void)attr;
    (void)state;
    return 0;
}
static inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
    (void)thread;
    (void)attr;
    (void)start_routine;
    (void)arg;
    return EPERM;
}
static inline int pthread_join(pthread_t thread, void **retval) {
    (void)thread;
    (void)retval;
    return EINVAL;
}

typedef _Atomic uint32_t pthread_mutex_t;
#define PTHREAD_MUTEX_INITIALIZER 0
int pthread_mutex_init(pthread_mutex_t *mutex, const void *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

typedef _Atomic uint32_t pthread_cond_t;
#define PTHREAD_COND_INITIALIZER 0
int pthread_cond_init(pthread_cond_t *cond, const void *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);

typedef _Atomic int pthread_condattr_t;
int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock_id);

typedef _Atomic uint32_t pthread_once_t;
#define PTHREAD_ONCE_INIT 0
int pthread_once(pthread_once_t *once_ctrl, void (*init_routine)(void));
