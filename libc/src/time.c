#include <time.h>
#include <stddef.h>
#include <stdint.h>
#include "arch.h"

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    tp->tv_sec = tv.tv_sec;
    tp->tv_nsec = tv.tv_usec * 1000;
    return 0;
}

int gettimeofday(struct timeval *tv, void *tz) {
    int64_t ts = __unixtime_usec();
    tv->tv_sec = ts / 1000000;
    tv->tv_usec = ts % 1000000;
    return 0;
}