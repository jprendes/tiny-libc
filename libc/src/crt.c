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
int128_t __divmodti4(int128_t num, int128_t den, int128_t *rem) {
    int8_t num_neg = num < 0 ? 1 : 0;
    int8_t den_neg = den < 0 ? 1 : 0;
    if (num_neg) num = -num;
    if (den_neg) den = -den;
    int128_t quot = __udivmodti4(num, den, (uint128_t*)rem);
    if (num_neg != den_neg) quot = -quot;
    if (rem && num_neg) *rem = -*rem;
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

__attribute__((used))
int128_t __divti3(int128_t a, int128_t b) {
  return __divmodti4(a, b, 0);
}

__attribute__((used))
int128_t __modti3(int128_t a, int128_t b) {
  int128_t r = 0;
  __divmodti4(a, b, &r);
  return r;
}
