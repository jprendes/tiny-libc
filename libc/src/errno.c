#include <errno.h>

static int errno_storage;

int *__errno_location(void) {
    return &errno_storage;
}
