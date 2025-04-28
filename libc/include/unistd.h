#pragma once

#include <stdint.h>

__attribute__((noreturn)) void _Exit(int ec);
ssize_t write(int fd, const void *buf, size_t count);
int putchar(int ch);

// readlink is used in the js_exepath function in quickjs-ng's cutils.c.
// However, js_exepath is only used from quickjs-libc.c, so it gets optimized away.
static inline ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) {
    return -1;
}