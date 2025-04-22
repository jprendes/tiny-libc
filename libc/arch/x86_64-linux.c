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

void _Exit(int ec) {
	syscall4(SYS_exit_group, ec, 0, 0, 0);
	for (;;) syscall4(SYS_exit, ec, 0, 0, 0);
}

const uint64_t SYS_write = 1;

ssize_t write(int fd, const void *buf, size_t count) {
    int64_t ret = syscall4(SYS_write, fd, (int64_t)buf, count, 0);
    return ret < 0 ? -1 : ret;
}

const uint64_t SYS_gettimeofday = 96;

int64_t unixtime_usec() {
    int64_t ts32[2];
    syscall4(SYS_gettimeofday, (int64_t)ts32, 0, 0, 0);
    return ts32[0] * 1000000 + ts32[1];
}

const uint64_t SYS_futex = 202;
const uint64_t FUTEX_WAIT = 0;
const uint64_t FUTEX_WAKE = 1;
const uint64_t FUTEX_PRIVATE = 128;

int futex_wait(uint32_t *uaddr, uint32_t val, uint64_t timeout_usec) {
    int64_t timeout[2] = {0, 0};
    int64_t *timeout_ptr = 0;
    if (timeout_usec > 0) {
        timeout[0] = timeout_usec / 1000000;
        timeout[1] = 1000 * (timeout_usec % 1000000);
        timeout_ptr = timeout;
    }
    syscall4(SYS_futex, (int64_t)uaddr, FUTEX_WAIT | FUTEX_PRIVATE, val, (int64_t)timeout_ptr);
    return 0;
}

int futex_wake(uint32_t *uaddr, size_t nwake) {
    syscall4(SYS_futex, (int64_t)uaddr, FUTEX_WAKE | FUTEX_PRIVATE, nwake, 0);
    return 0;
}
