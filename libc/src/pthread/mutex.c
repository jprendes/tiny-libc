#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>

#include "futex.h"

enum {
    UNLOCKED = PTHREAD_MUTEX_INITIALIZER,
    LOCKED_NO_WAIT,
    LOCKED_WAIT,
};

uint32_t atomic_compare_swap(pthread_mutex_t *m, uint32_t expected, uint32_t desired) {
    uint32_t v = expected;
    atomic_compare_exchange_strong(m, &v, desired);
    return v;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const void *attr) {
  atomic_store(mutex, UNLOCKED);
  return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    return pthread_mutex_timedlock(mutex, NULL);
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime) {
    uint32_t c = UNLOCKED;
    if (atomic_compare_exchange_strong(mutex, &c, LOCKED_NO_WAIT)) {
      // successfully acquired the lock
      return 0;
    }
    // someone else holds the lock
    do {
      // bump c up to LOCKED_WAIT, if necessary
      if (c == LOCKED_WAIT ||
          // we're the first waiter, set m to LOCKED_WAIT
          // XXX: the comparison in this line is extremely confusing
          atomic_compare_swap(mutex, LOCKED_NO_WAIT, LOCKED_WAIT) != UNLOCKED) {
        if (futex_wait_abs(mutex, LOCKED_WAIT, abstime) == ETIMEDOUT) {
            return ETIMEDOUT;
        }
      }
      // subtlety: sets the expected value for the next call to cmpxchg in the
      // while condition
      c = UNLOCKED;
    }
    // change the futex state from UNLOCKED to LOCKED_WAIT (fails if futex is
    // locked in between loop body and this check); we set it to LOCKED_WAIT
    // because we know there are other waiters even after we acquire the lock
    while (!atomic_compare_exchange_weak(mutex, &c, LOCKED_WAIT));
    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
    uint32_t c = UNLOCKED;
    if (!atomic_compare_exchange_strong(mutex, &c, LOCKED_NO_WAIT)) {
        // someone else holds the lock
        return EBUSY;
    }
    // successfully acquired the lock
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    // note that subtracting 1 either turns LOCKED_WAIT into LOCKED_NO_WAIT (in
    // which case the body runs because we need to wait up at least one waiter),
    // or it turns LOCKED_NO_WAIT into UNLOCKED (in which case we've already
    // released the lock)
    if (atomic_fetch_sub(mutex, 1) != LOCKED_NO_WAIT) {
        atomic_store(mutex, UNLOCKED);
        futex_wake((uint32_t*)mutex, 1);
    }
    return 0;
}
