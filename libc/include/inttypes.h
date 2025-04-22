#pragma once

#include <stdint.h>

#if UINTPTR_MAX == UINT64_MAX
#define __PRI64  "l"
#define __PRIPTR "l"
#else
#define __PRI64  "ll"
#define __PRIPTR ""
#endif

#define PRId64 __PRI64 "d"
#define PRIu64 __PRI64 "u"
#define PRIx64 __PRI64 "x"
