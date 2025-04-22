#include "libm.h"

double __math_xflow(uint32_t sign, double y)
{
	return (sign ? -y : y) * y;
}
