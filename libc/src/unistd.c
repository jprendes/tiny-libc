#include <unistd.h>
#include <stdio.h>
#include "arch.h"

__attribute__((noreturn)) void _Exit(int ec) {
    __exit(ec);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return __write(fd, buf, count);
}

int putchar(int ch) {
    return fputc(ch, stdout);
}
