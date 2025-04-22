#include <stdlib.h>
#include <string.h>

void *realloc(void *ptr, size_t new_size) {
    if (!ptr) {
        return malloc(new_size);
    }
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    size_t old_size = malloc_usable_size(ptr);
    if (old_size >= new_size) {
        return ptr;
    }
    void *new_ptr = malloc(new_size);
    if (!new_ptr) {
        return NULL;
    }
    __builtin_memcpy(new_ptr, ptr, old_size);
    free(ptr);
    return new_ptr;
}

void *calloc(size_t num, size_t size) {
    volatile size_t len = num * size;
    void *ptr = malloc(len);
    __builtin_memset(ptr, '\0', num * size);
    return ptr;
}

void abort() {
    exit(137);
}
