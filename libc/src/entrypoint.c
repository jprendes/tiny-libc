#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>
#include "arch.h"

int main();

typedef void (*fn)();

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

__attribute__((force_align_arg_pointer))
void _start() {
    atexit(finalizer);
    initializer();
    exit(main());
}

// linked list of functions to call at exit
static struct at_exit_entry {
    fn func;
    struct at_exit_entry *next;
} *at_exit_list = NULL;

void exit(int status) {
    struct at_exit_entry *head = (struct at_exit_entry *)atomic_load((intptr_t*)&at_exit_list);
    while (head) {
        struct at_exit_entry *next = head->next;
        head->func();
        free(head);
        head = next;
    }
    _Exit(status);
}

// register a function to be called at exit
int atexit(void (*func)(void)) {
    struct at_exit_entry *entry = malloc(sizeof(struct at_exit_entry));
    if (!entry) return -1;
    entry->func = func;
    for (;;) {
        // atromically add the entry to the list
        // if the list has changed, retry
        struct at_exit_entry *head = (struct at_exit_entry *)atomic_load((intptr_t*)&at_exit_list);
        entry->next = head;
        if (atomic_compare_exchange_strong((intptr_t*)&at_exit_list, (intptr_t*)&head, (intptr_t)entry)) {
            break;
        }
    }
    return 0;
}
