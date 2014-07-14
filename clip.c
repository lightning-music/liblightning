
#include "clip.h"
#include "types.h"

sample_t
clip(sample_t val, sample_t min, sample_t max) {
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}
