#pragma once

#include <stdint.h>

typedef int64_t time_t;

struct timeval {
  time_t tv_sec;
  time_t tv_usec;
};

int gettimeofday(struct timeval *tv, void *tz);

struct timespec {
  time_t tv_sec;
  time_t tv_nsec;
};

typedef int clockid_t;

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1

int clock_gettime(clockid_t clk_id, struct timespec *tp);

struct tm
{
  int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
  int tm_min;			/* Minutes.	[0-59] */
  int tm_hour;			/* Hours.	[0-23] */
  int tm_mday;			/* Day.		[1-31] */
  int tm_mon;			/* Month.	[0-11] */
  int tm_year;			/* Year	- 1900.  */
  int tm_wday;			/* Day of week.	[0-6] */
  int tm_yday;			/* Days in year.[0-365]	*/
  int tm_isdst;			/* DST.		[-1/0/1]*/

  long int tm_gmtoff;		/* Seconds east of UTC.  */
  const char *tm_zone;		/* Timezone abbreviation.  */
};

struct tm* localtime_r(const time_t* timer, struct tm* buf);
