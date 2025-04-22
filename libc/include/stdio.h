#pragma once

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdarg.h>

#define EOF (-1)

struct FILE;

typedef struct FILE FILE;

FILE *__stdout();
#define stdout (__stdout())

FILE *__stderr();
#define stderr (__stderr())

int fflush(FILE *fp);
int fputc(int c, FILE *fp);
int fputs(const char *s, FILE * fp);

int __attribute__((format(printf, 1, 2))) printf(const char* fmt, ...);
int __attribute__((format(printf, 2, 3))) fprintf(FILE * fp, const char* fmt, ...);
int __attribute__((format(printf, 3, 4))) snprintf(char* buffer, size_t count, const char* fmt, ...);
int vfprintf(FILE * fp, const char* fmt, va_list va);
int vsnprintf(char* buffer, size_t count, const char* fmt, va_list va);

int puts(const char *s);
size_t fwrite(const void* buffer, size_t size, size_t count, FILE* stream );
