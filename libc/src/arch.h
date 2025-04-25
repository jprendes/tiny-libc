#include <stdint.h>

void _Exit(int ec);
int64_t unixtime_usec();
ssize_t write(int fd, const void *buf, size_t count);
int futex_wait(uint32_t *uaddr, uint32_t val, uint64_t timeout_usec);
int futex_wake(uint32_t *uaddr, size_t nwake);
void *__page_alloc(size_t page_n_bits, size_t n_pages);
