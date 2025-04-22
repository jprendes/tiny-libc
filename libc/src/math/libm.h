#ifndef _LIBM_H
#define _LIBM_H

#include <stdint.h>
#include <float.h>
#include <math.h>

/* Helps static branch prediction so hot path can be better optimized.  */
#ifdef __GNUC__
#define predict_true(x) __builtin_expect(!!(x), 1)
#define predict_false(x) __builtin_expect(x, 0)
#else
#define predict_true(x) (x)
#define predict_false(x) (x)
#endif

#define asuint(f) ((union{float _f; uint32_t _i;}){f})._i
#define asfloat(i) ((union{uint32_t _i; float _f;}){i})._f
#define asuint64(f) ((union{double _f; uint64_t _i;}){f})._i
#define asdouble(i) ((union{uint64_t _i; double _f;}){i})._f

#define EXTRACT_WORDS(hi,lo,d)                    \
do {                                              \
  uint64_t __u = asuint64(d);                     \
  (hi) = __u >> 32;                               \
  (lo) = (uint32_t)__u;                           \
} while (0)

#define GET_HIGH_WORD(hi,d)                       \
do {                                              \
  (hi) = asuint64(d) >> 32;                       \
} while (0)

#define GET_LOW_WORD(lo,d)                        \
do {                                              \
  (lo) = (uint32_t)asuint64(d);                   \
} while (0)

#define INSERT_WORDS(d,hi,lo)                     \
do {                                              \
  (d) = asdouble(((uint64_t)(hi)<<32) | (uint32_t)(lo)); \
} while (0)

#define SET_HIGH_WORD(d,hi)                       \
  INSERT_WORDS(d, hi, (uint32_t)asuint64(d))

#define SET_LOW_WORD(d,lo)                        \
  INSERT_WORDS(d, asuint64(d)>>32, lo)

#define GET_FLOAT_WORD(w,d)                       \
do {                                              \
  (w) = asuint(d);                                \
} while (0)

#define SET_FLOAT_WORD(d,w)                       \
do {                                              \
  (d) = asfloat(w);                               \
} while (0)

__attribute__((__visibility__("hidden"))) int    __rem_pio2_large(double*,double*,int,int,int);

__attribute__((__visibility__("hidden"))) int    __rem_pio2(double,double*);
__attribute__((__visibility__("hidden"))) double __sin(double,double,int);
__attribute__((__visibility__("hidden"))) double __cos(double,double);
__attribute__((__visibility__("hidden"))) double __tan(double,double,int);
__attribute__((__visibility__("hidden"))) double __expo2(double,double);

__attribute__((__visibility__("hidden"))) int    __rem_pio2f(float,double*);
__attribute__((__visibility__("hidden"))) float  __sindf(double);
__attribute__((__visibility__("hidden"))) float  __cosdf(double);
__attribute__((__visibility__("hidden"))) float  __tandf(double,int);
__attribute__((__visibility__("hidden"))) float  __expo2f(float,float);

__attribute__((__visibility__("hidden"))) int __rem_pio2l(long double, long double *);
__attribute__((__visibility__("hidden"))) long double __sinl(long double, long double, int);
__attribute__((__visibility__("hidden"))) long double __cosl(long double, long double);
__attribute__((__visibility__("hidden"))) long double __tanl(long double, long double, int);

__attribute__((__visibility__("hidden"))) long double __polevll(long double, const long double *, int);
__attribute__((__visibility__("hidden"))) long double __p1evll(long double, const long double *, int);

extern int __signgam;
__attribute__((__visibility__("hidden"))) double __lgamma_r(double, int *);
__attribute__((__visibility__("hidden"))) float __lgammaf_r(float, int *);

/* error handling functions */
__attribute__((__visibility__("hidden"))) float __math_xflowf(uint32_t, float);
__attribute__((__visibility__("hidden"))) float __math_uflowf(uint32_t);
__attribute__((__visibility__("hidden"))) float __math_oflowf(uint32_t);
__attribute__((__visibility__("hidden"))) float __math_divzerof(uint32_t);
__attribute__((__visibility__("hidden"))) float __math_invalidf(float);
__attribute__((__visibility__("hidden"))) double __math_xflow(uint32_t, double);
__attribute__((__visibility__("hidden"))) double __math_uflow(uint32_t);
__attribute__((__visibility__("hidden"))) double __math_oflow(uint32_t);
__attribute__((__visibility__("hidden"))) double __math_divzero(uint32_t);
__attribute__((__visibility__("hidden"))) double __math_invalid(double);
#if LDBL_MANT_DIG != DBL_MANT_DIG
__attribute__((__visibility__("hidden"))) long double __math_invalidl(long double);
#endif

#endif
