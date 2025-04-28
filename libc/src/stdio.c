#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "printf.h"

struct range {
    char *start;
    char *end;
};

struct file_buffer {
    struct range allocated;
    struct range used;
    pthread_mutex_t mutex;
};

struct FILE {
    int fd;
    struct file_buffer in;
    struct file_buffer out;
};

#define MAKE_FILE(name, fd_num, in_buffer_size, out_buffer_size) \
FILE *name(void) { \
    static char name##_out_buffer[out_buffer_size] = {0}; \
    static char name##_in_buffer[in_buffer_size] = {0}; \
    static struct FILE name##_file = { \
        .fd = fd_num, \
        .in = { \
            .allocated = { \
                .start = name##_in_buffer, \
                .end = name##_in_buffer + sizeof(name##_in_buffer), \
            }, \
            .used = { \
                .start = name##_in_buffer, \
                .end = name##_in_buffer, \
            }, \
            .mutex = PTHREAD_MUTEX_INITIALIZER, \
        }, \
        .out = { \
            .allocated = { \
                .start = name##_out_buffer, \
                .end = name##_out_buffer + sizeof(name##_out_buffer), \
            }, \
            .used = { \
                .start = name##_out_buffer, \
                .end = name##_out_buffer, \
            }, \
            .mutex = PTHREAD_MUTEX_INITIALIZER, \
        }, \
    }; \
    return &name##_file; \
}

MAKE_FILE(__stdin, 0, 1024, 1024)
MAKE_FILE(__stdout, 1, 1024, 1024)
MAKE_FILE(__stderr, 2, 1024, 1024)

static inline int fflush_unlocked(FILE *fp) {
    if (fp == NULL) return EOF;
    int fd = fp->fd;
    struct range *used = &fp->out.used;
    while (used->start < used->end) {
        int r = write(fd, used->start, used->end - used->start);
        if (r < 0) {
            return EOF;
        }
        used->start += r;
    }
    used->start = used->end = fp->out.allocated.start;
    return 0;
}

int fflush(FILE *fp) {
    pthread_mutex_lock(&fp->out.mutex);
    int ret = fflush_unlocked(fp);
    pthread_mutex_unlock(&fp->out.mutex);
    return ret;
}

static inline size_t fputbuf_unlocked(const char *buf, size_t n, FILE *fp) {
    if (fp->out.used.end >= fp->out.allocated.end) {
        if (fflush_unlocked(fp) == EOF) return 0;
    }
    for (size_t i = 0; i < n; i++) {
        char c = *buf++;
        *fp->out.used.end++ = c;
        if (c == '\n' || fp->out.used.end >= fp->out.allocated.end) {
            if (fflush_unlocked(fp) == EOF) return i + 1;
        }
    }
    return n;
}

static inline size_t fgetbuf_unlocked(char *buf, size_t n, FILE *fp) {
    struct range *used = &fp->in.used;
    if (used->start == used->end) {
        used->start = used->end = fp->in.allocated.start;
        ssize_t r = read(fp->fd, used->start, fp->in.allocated.end - used->end);
        if (r <= 0) return 0;
        used->end += r;
    }
    for (size_t i = 0; i < n; i++) {
        char c = *used->start++;
        *buf++ = c;
        if (c == '\n' || used->start == used->end) {
            return i + 1;
        }
    }
    return n;
}

int fputc(int c, FILE *fp) {
    pthread_mutex_lock(&fp->out.mutex);
    char cc = (char)c;
    size_t n = fputbuf_unlocked(&cc, 1, fp);;
    pthread_mutex_unlock(&fp->out.mutex);
    return n == 0 ? EOF : (uint8_t)cc;
}

static inline int fputs_unlocked(const char *s, FILE * fp) {
    size_t n = strlen(s);
    if (fputbuf_unlocked((char*)s, n, fp) == 0) return EOF;
    if (fputbuf_unlocked("\n", 1, fp) == 0) return EOF;
    return n + 1;
}

int fputs(const char *s, FILE * fp) {
    pthread_mutex_lock(&fp->out.mutex);
    int ret = fputs_unlocked(s, fp);
    pthread_mutex_unlock(&fp->out.mutex);
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
    fputbuf_unlocked(&character, 1, (FILE*)buffer);
}

int vfprintf(FILE * fp, const char* fmt, va_list va) {
    pthread_mutex_lock(&fp->out.mutex);
    int ret = _vsnprintf(_out_file, (char*)fp, (size_t)-1, fmt, va);
    pthread_mutex_unlock(&fp->out.mutex);
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

static inline size_t fwrite_unlocked(const void* buffer, size_t size, size_t count, FILE* fp) {
    size_t n = fputbuf_unlocked((char*)buffer, size * count, fp);
    return n / size;
}

size_t fwrite(const void* buffer, size_t size, size_t count, FILE* fp) {
    pthread_mutex_lock(&fp->out.mutex);
    size_t ret = fwrite_unlocked(buffer, size, count, fp);
    pthread_mutex_unlock(&fp->out.mutex);
    return ret;
}

static inline int fgetc_unlocked(FILE* fp) {
    char c = 0;
    if (fgetbuf_unlocked(&c, 1, fp) == 0) return EOF;
    return (uint8_t)c;
}

int fgetc(FILE* fp) {
    pthread_mutex_lock(&fp->in.mutex);
    char c = 0;
    size_t n = fgetbuf_unlocked(&c, 1, fp);
    pthread_mutex_unlock(&fp->in.mutex);
    return n == 0 ? EOF : (uint8_t)c;
}

static inline size_t fread_unlocked(void *buffer, size_t size, size_t count, FILE *fp) {
    size_t n = fgetbuf_unlocked(buffer, size * count, fp);
    return n / size;
}

size_t fread(void *buffer, size_t size, size_t count, FILE *fp) {
    pthread_mutex_lock(&fp->in.mutex);
    size_t ret = fread_unlocked(buffer, size, count, fp);
    pthread_mutex_unlock(&fp->in.mutex);
    return ret;
}

int putchar(int ch) {
    return fputc(ch, stdout);
}

int getchar(void) {
    return fgetc(stdin);
}
