#include <fenv.h>

int fesetround(int round) {
    (void)round;
    return 0;
}
