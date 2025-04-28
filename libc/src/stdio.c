#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "printf.h"

struct FILE {
    int fd;
    pthread_mutex_t mutex;
    int out_buffer_pos;
    int out_buffer_size;
    char out_buffer[1];
};

#define MAKE_FILE(name, fd_num, buffer_size) \
FILE *name(void) { \
    static struct { \
        FILE f; \
        char buffer[buffer_size]; \
    } THE_FILE = { \
        .f = { \
            .fd = fd_num, \
            .mutex = PTHREAD_MUTEX_INITIALIZER, \
            .out_buffer_pos = 0, \
            .out_buffer_size = buffer_size, \
        } \
    }; \
    return &THE_FILE.f; \
}

MAKE_FILE(__stdin, 0, 1024)
MAKE_FILE(__stdout, 1, 1024)
MAKE_FILE(__stderr, 2, 1024)

int fflush_unlocked(FILE *fp) {
    if (fp == NULL) return EOF;
    int fd = fp->fd;
    char *buf = fp->out_buffer;
    int n = fp->out_buffer_pos;
    int k = 0;
    while (k < n) {
        int r = write(fd, buf + k, n - k);
        if (r < 0) {
            return EOF;
        }
        k += r;
    }
    fp->out_buffer_pos = 0;
    return 0;
}

int fflush(FILE *fp) {
    pthread_mutex_lock(&fp->mutex);
    int ret = fflush_unlocked(fp);
    pthread_mutex_unlock(&fp->mutex);
    return ret;
}

int fputc_unlocked(int c, FILE *fp) {
    if (fp->out_buffer_pos >= fp->out_buffer_size) {
        // this should not happen, but just in case flush the buffer
        if (fflush_unlocked(fp) == EOF) return EOF;
    }
    fp->out_buffer[fp->out_buffer_pos++] = c;
    if (c == '\n' || fp->out_buffer_pos >= fp->out_buffer_size) {
        if (fflush_unlocked(fp) == EOF) return EOF;
    }
    return c;
}

int fputc(int c, FILE *fp) {
    pthread_mutex_lock(&fp->mutex);
    int ret = fputc_unlocked(c, fp);
    pthread_mutex_unlock(&fp->mutex);
    return ret;
}

int fputs_unlocked(const char *s, FILE * fp) {
    size_t i = 0;
    while (s[i] != '\0') {
        if (fputc_unlocked(s[i++], fp) == EOF) {
            return EOF;
        }
    }
    if (fputc_unlocked('\n', fp) == EOF) {
        return EOF;
    }
    return i + 1;
}

int fputs(const char *s, FILE * fp) {
    pthread_mutex_lock(&fp->mutex);
    int ret = fputs_unlocked(s, fp);
    pthread_mutex_unlock(&fp->mutex);
    return ret;
}

int printf(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int res = vfprintf(__stdout(), fmt, va);
    va_end(va);
    return res;
}

int fprintf(FILE * fp, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int res = vfprintf(fp, fmt, va);
    va_end(va);
    return res;
}

int snprintf(char* buffer, size_t count, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int res = vsnprintf(buffer, count, fmt, va);
    va_end(va);
    return res;
}

static inline void _out_file(char character, void* buffer, size_t idx, size_t maxlen) {
    (void)idx;
    (void)maxlen;
    fputc_unlocked(character, (FILE*)buffer);
}

int vfprintf(FILE * fp, const char* fmt, va_list va) {
    pthread_mutex_lock(&fp->mutex);
    int ret = _vsnprintf(_out_file, (char*)fp, (size_t)-1, fmt, va);
    pthread_mutex_unlock(&fp->mutex);
    return ret;
}

static inline void _out_buffer(char character, void* buffer, size_t idx, size_t maxlen) {
    if (idx < maxlen) ((char*)buffer)[idx] = character;
}

int vsnprintf(char* buffer, size_t count, const char* fmt, va_list va) {
    return _vsnprintf(_out_buffer, buffer, count, fmt, va);
}

int puts(const char *s) {
    return fputs(s, __stdout());
}

size_t fwrite_unlocked(const void* buffer, size_t size, size_t count, FILE* fp) {
    char * buf = (char*)buffer;
    size_t total = size * count;
    size_t written = 0;
    while (written < total) {
        if (fputc_unlocked(buf[written], fp) == EOF) return written;
        written++;
    }
    return written;
}

size_t fwrite(const void* buffer, size_t size, size_t count, FILE* fp) {
    pthread_mutex_lock(&fp->mutex);
    size_t ret = fwrite_unlocked(buffer, size, count, fp);
    pthread_mutex_unlock(&fp->mutex);
    return ret;
}
