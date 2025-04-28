#include <stdint.h>
#include <time.h>
#include <errno.h>

#include "../arch.h"

static inline int __wait_abs(_Atomic uint32_t *uaddr, uint32_t val, const struct timespec *abstime) {
    if (abstime == NULL) {
        return __wait((uint32_t *)uaddr, val, 0);
    }

    int64_t abstime_usec = (int64_t)abstime->tv_sec * 1000000 + (int64_t)abstime->tv_nsec / 1000;
    int64_t reltime_usec = abstime_usec - __unixtime_usec();

    if (reltime_usec <= 0) return ETIMEDOUT;

    __wait((uint32_t *)uaddr, val, reltime_usec);

    return 0;
}
