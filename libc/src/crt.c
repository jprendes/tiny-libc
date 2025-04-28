#include <stdint.h>
#include <stdio.h>

static inline uint8_t clz128(uint128_t x) {
  if (x == 0) return 128;
  if (x >> 64) return __builtin_clzll(x >> 64);
  return 64 + __builtin_clzll(x);
}

__attribute__((used))
uint128_t __udivmodti4(uint128_t num, uint128_t den, uint128_t *rem) {
  uint128_t quot = 0, qbit = 1;

	if (den == 0) return (uint64_t)num/(uint64_t)den;

  uint8_t n = clz128(den);
  den <<= n;
  qbit <<= n;

	while (qbit) {
		if (den <= num) {
			num -= den;
			quot += qbit;
		}
		den >>= 1;
		qbit >>= 1;
	}

	if (rem) *rem = num;

	return quot;
}

__attribute__((used))
uint128_t __udivti3(uint128_t a, uint128_t b) {
  return __udivmodti4(a, b, 0);
}

__attribute__((used))
uint128_t __umodti3(uint128_t a, uint128_t b) {
  uint128_t r = 0;
  __udivmodti4(a, b, &r);
  return r;
}
