#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "arch.h"

void __assert(const char *msg, const char *file, int line) {
    printf("Assertion failed: %s at %s, line %d", msg, file, line);
    _Exit(1);
}
