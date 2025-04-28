#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include "arch.h"

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    (void)clk_id;
    int128_t ts = __unixtime_nsec();
    tp->tv_sec = ts / 1000000000;
    tp->tv_nsec = ts % 1000000000;
    return 0;
}

int gettimeofday(struct timeval *tv, void *tz) {
    (void)tz;
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    tv->tv_sec = tp.tv_sec;
    tv->tv_usec = tp.tv_nsec / 1000;
    return 0;
}
