#pragma once

#include <stdint.h>
#include <stddef.h>

size_t strlen(const char *str);
void *memcpy(void *dest, const void *src, size_t len);
void *memmove(void *dest, const void *src, size_t len);
void *memset(void *dest, int c, size_t len);
int memcmp(const void *s1, const void *s2, size_t len);
int bcmp(const void *v1, const void *v2, size_t len);
void *memchr(const void*ptr, int c, size_t len );
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
int strcmp(const char *s1, const char *s2);
char *strcat(char *dest, const char *src);
char* strstr(const char* str, const char* substr);
int strncmp(const char *s1, const char *s2, size_t n);

int atoi(const char *p);

char *strcpy(char *dest, const char *src);
