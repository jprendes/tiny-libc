#pragma once

#define NAN       __builtin_nanf("")
#define INFINITY  __builtin_inff()

#define isnan(x) __builtin_isnan(x)
#define isinf(x) __builtin_isinf(x)
#define isfinite(x) __builtin_isfinite(x)
#define signbit(x) __builtin_signbit(x)

double      acos(double);
double      acosh(double);
double      asin(double);
double      asinh(double);
double      atan(double);
double      atan2(double, double);
double      atanh(double);
double      cbrt(double);
double      ceil(double);
double      cos(double);
double      cosh(double);
double      copysign(double, double);
double      exp(double);
double      expm1(double);
double      fabs(double);
double      floor(double);
double      fmax(double, double);
double      fmin(double, double);
double      fmod(double, double);
double      frexp(double, int *);
double      hypot(double, double);
double      log(double);
double      log10(double);
double      log1p(double);
double      log2(double);
long        lrint(double);
double      modf(double, double *);
double      pow(double, double);
double      rint(double);
double      round(double);
double      scalbn(double, int);
double      sin(double);
double      sinh(double);
double      sqrt(double);
double      tan(double);
double      tanh(double);
double      trunc(double);
