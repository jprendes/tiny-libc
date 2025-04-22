#pragma once

#include <stdint.h>

intptr_t buf[5]; // see https://gcc.gnu.org/onlinedocs/gcc/Nonlocal-Gotos.html

#define setjmp __builtin_setjmp
#define longjmp __builtin_longjmp
