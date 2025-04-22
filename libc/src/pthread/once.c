#include <pthread.h>
#include <stdatomic.h>
#include <stddef.h>
#include "futex.h"

enum {
    UNINITIALIZED = PTHREAD_ONCE_INIT,
    PENDING,
    INITIALIZED,
};

int pthread_once(pthread_once_t *once_ctrl, void (*init_routine)(void)) {
    uint32_t c = UNINITIALIZED;
    atomic_compare_exchange_strong(once_ctrl, &c, PENDING);
    switch (c) {
        case UNINITIALIZED:
            // The first thread to reach here will run the init_routine.
            init_routine();
            atomic_store(once_ctrl, INITIALIZED);
            futex_wake((uint32_t*)once_ctrl, INT32_MAX);
            return 0;
        case PENDING:
            // Another thread is already running the init_routine.
            // Wait for it to finish.
            futex_wait((uint32_t*)once_ctrl, PENDING, 0);
            return 0;
        case INITIALIZED:
            // The init_routine has already been run.
            return 0;
    }
    return 0;
}
