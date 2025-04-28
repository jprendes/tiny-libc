#include <stdint.h>
#include <time.h>
#include <errno.h>

#include "../arch.h"

static inline int __wait_abs(_Atomic uint32_t *uaddr, uint32_t val, const struct timespec *abstime) {
    if (abstime == NULL) {
        return __wait((uint32_t *)uaddr, val, 0);
    }

    int128_t abstime_nsec = (int64_t)abstime->tv_sec * 1000000000 + (int64_t)abstime->tv_nsec;
    int128_t reltime_nsec = abstime_nsec - __unixtime_nsec();

    if (reltime_nsec <= 0) return ETIMEDOUT;

    __wait((uint32_t *)uaddr, val, reltime_nsec);

    return 0;
}
