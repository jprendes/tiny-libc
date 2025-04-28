#include <stdint.h>

int64_t syscall4(uint64_t n, int64_t a1, int64_t a2, int64_t a3, int64_t a4) {
    int64_t ret;
	register int64_t r10 __asm__("r10") = a4;
	__asm__ __volatile__ (
        "syscall"
        : "=a"(ret)
        : "a"(n), "D"(a1), "S"(a2), "d"(a3), "r"(r10)
        : "rcx", "r11", "memory");
	return ret;
}

const uint64_t SYS_exit_group = 231;
const uint64_t SYS_exit = 60;

__attribute__((noreturn)) void __exit(int ec) {
	syscall4(SYS_exit_group, ec, 0, 0, 0);
	for (;;) syscall4(SYS_exit, ec, 0, 0, 0);
}

const uint64_t SYS_write = 1;

ssize_t __write(int fd, const void *buf, size_t count) {
    int64_t ret = syscall4(SYS_write, fd, (int64_t)buf, count, 0);
    return ret < 0 ? -1 : ret;
}

const uint64_t SYS_read = 0;

ssize_t __read(int fd, void *buf, size_t count) {
    int64_t ret = syscall4(SYS_read, fd, (int64_t)buf, count, 0);
    return ret < 0 ? -1 : ret;
}

const uint64_t SYS_clock_gettime64 = 228;
const uint64_t CLOCK_MONOTONIC = 1;

int128_t __unixtime_nsec(void) {
    int64_t ts[2];
    syscall4(SYS_clock_gettime64, CLOCK_MONOTONIC, (int64_t)ts, 0, 0);
    return (int128_t)ts[0] * 1000000000 + ts[1];
}

const uint64_t SYS_futex = 202;
const uint64_t FUTEX_WAIT = 0;
const uint64_t FUTEX_WAKE = 1;
const uint64_t FUTEX_PRIVATE = 128;

int __wait(uint32_t *uaddr, uint32_t val, uint128_t timeout_nsec) {
    int64_t timeout[2] = {0, 0};
    int64_t *timeout_ptr = 0;
    if (timeout_nsec > 0) {
        timeout[0] = timeout_nsec / 1000000000;
        timeout[1] = timeout_nsec % 1000000000;
        timeout_ptr = timeout;
    }
    syscall4(SYS_futex, (int64_t)uaddr, FUTEX_WAIT | FUTEX_PRIVATE, val, (int64_t)timeout_ptr);
    return 0;
}

int __wake(uint32_t *uaddr, size_t nwake) {
    syscall4(SYS_futex, (int64_t)uaddr, FUTEX_WAKE | FUTEX_PRIVATE, nwake, 0);
    return 0;
}

#ifndef STATIC_ALLOCATION

// use the kernel to allocate heap memory

const uint64_t SYS_brk = 12;

static inline void *brk(void *addr) {
    return (void *)syscall4(SYS_brk, (int64_t)addr, 0, 0, 0);
}

#else

// use a predefined static buffer for heap memory

#ifndef HEAP_SIZE
#define HEAP_SIZE (32 * 1024 * 1024)
#endif

#define clamp(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

static inline void *brk(void *addr) {
    static uint8_t heap[HEAP_SIZE];
    static void *current = heap;
    if (addr) {
        current = clamp(addr, (void*)heap, (void*)heap + sizeof(heap));
    }
    return current;
}

#endif

static char *heap_base = 0;
static char *heap_end = 0;
static char *brk_end = 0;

void *__page_alloc(size_t page_n_bits, size_t n_pages) {
    if (!heap_base) {
        heap_base = heap_end = brk_end = brk(0);
        if (!heap_base) {
            return 0;
        }
    }

    size_t page_size = 1 << page_n_bits;

    // align up to page_size
    char *ptr = heap_end + ( -(intptr_t)heap_end & (page_size-1) );
        
    heap_end = ptr + page_size * n_pages;
    if (heap_end > brk_end) {
        brk_end = brk(heap_end);
        if (brk_end < heap_end) {
            return 0;
        }
    }

    return ptr;
}
