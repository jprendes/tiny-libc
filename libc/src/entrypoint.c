#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include "arch.h"

int main(void);

typedef void (*fn)(void);

// the linker defines these symbols for us
extern fn __attribute__((weak)) __preinit_array_start[];
extern fn __attribute__((weak)) __preinit_array_end[];
extern fn __attribute__((weak)) __init_array_start[];
extern fn __attribute__((weak)) __init_array_end[];
extern fn __attribute__((weak)) __fini_array_start[];
extern fn __attribute__((weak)) __fini_array_end[];
extern void __attribute__((weak)) _fini(void);
extern void __attribute__((weak)) _init(void);

static void finalizer(void) {
    if (__fini_array_end) {
        for (fn *p_fn = __fini_array_end - 1; p_fn >= __fini_array_start; p_fn--) {
            if (*p_fn != (fn)0 && *p_fn != (fn)1)
                (*p_fn)();
        }
    }
    if (_fini) _fini();
    fflush(stdout);
    fflush(stderr);
}

static void initializer(void) {
    if (__preinit_array_start) {
        for (fn *p_fn = __preinit_array_start; p_fn < __preinit_array_end; p_fn++) {
            if (*p_fn != (fn)0 && *p_fn != (fn)1)
                (*p_fn)();
        }
    }

    if (_init) _init();

    if (__init_array_start) {
        for (fn *p_fn = __init_array_start; p_fn < __init_array_end; p_fn++) {
            if (*p_fn != (fn)0 && *p_fn != (fn)1)
                (*p_fn)();
        }
    }
}

void *__stack_chk_guard = (void *)0xdeadbeef;
void __stack_chk_fail(void) {
    puts("stack overflow detected");
    abort();
}

__attribute__((force_align_arg_pointer))
void _start(void) {
    initializer();
    exit(main());
}

// linked list of functions to call at exit
static struct at_exit_entry {
    fn func;
    struct at_exit_entry *next;
} *at_exit_list = NULL;

pthread_mutex_t at_exit_mutex = PTHREAD_MUTEX_INITIALIZER;

__attribute__((noreturn)) void exit(int status) {
    // no need to deallocate the at_exit_list, since we are exiting anyways
    pthread_mutex_lock(&at_exit_mutex);
    while (at_exit_list) {
        struct at_exit_entry *next = at_exit_list->next;
        at_exit_list->func();
        free(at_exit_list);
        at_exit_list = next;
    }
    finalizer();
    _Exit(status);
}

// register a function to be called at exit
int atexit(void (*func)(void)) {
    struct at_exit_entry *entry = malloc(sizeof(struct at_exit_entry));
    if (!entry) return -1;
    entry->func = func;
    pthread_mutex_lock(&at_exit_mutex);
    entry->next = at_exit_list;
    at_exit_list = entry;
    pthread_mutex_unlock(&at_exit_mutex);
    return 0;
}
