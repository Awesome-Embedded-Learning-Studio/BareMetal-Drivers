#pragma once
#include <limits.h>
#include <stdint.h>

#include "cfbd_graphic_define.h"

static inline int32_t asInt32_t(PointBaseType v)
{
    return (int32_t) v;
}

static inline PointBaseType clamp_u16_from_i32(int32_t v)
{
    if (v < 0)
        return 0;
    if (v > INT_MAX)
        v = INT_MAX;
    if (v > UINT16_MAX)
        return UINT16_MAX;
    return (PointBaseType) v;
}