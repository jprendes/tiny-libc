#include "libm.h"

double __math_divzero(uint32_t sign)
{
	return (sign ? -1.0 : 1.0) / 0.0;
}
