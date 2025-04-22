#pragma once

#include <stdint.h>

void *malloc(size_t size);
void *realloc(void *ptr, size_t new_size);
void *calloc(size_t num, size_t size);
void free(void *ptr);
size_t malloc_usable_size(void *ptr);
void *alloca(size_t size); /// magically implemented by the compiler