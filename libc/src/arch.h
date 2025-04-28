#include <stdint.h>

__attribute__((noreturn)) void __exit(int ec);
int128_t __unixtime_nsec(void);
ssize_t __write(int fd, const void *buf, size_t count);
ssize_t __read(int fd, void *buf, size_t count);
int __wait(uint32_t *uaddr, uint32_t val, uint128_t timeout_nsec);
int __wake(uint32_t *uaddr, size_t nwake);
void *__page_alloc(size_t page_n_bits, size_t n_pages);
