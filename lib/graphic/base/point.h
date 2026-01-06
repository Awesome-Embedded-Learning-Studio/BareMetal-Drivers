#pragma once
#include "cfbd_graphic_define.h"

typedef struct
{
    PointBaseType x;
    PointBaseType y;
} CFBDGraphic_Point;

typedef struct
{
    int32_t x;
    int32_t y;
} CFBDGraphic_Vec2i;

static inline CFBDGraphic_Point point_add(CFBDGraphic_Point a, CFBDGraphic_Point b)
{
    uint32_t x = (uint32_t) a.x + b.x;
    uint32_t y = (uint32_t) a.y + b.y;

    if (x > UINT16_MAX)
        x = UINT16_MAX;
    if (y > UINT16_MAX)
        y = UINT16_MAX;

    return (CFBDGraphic_Point) {(uint16_t) x, (uint16_t) y};
}

static inline CFBDGraphic_Vec2i point_sub(CFBDGraphic_Point a, CFBDGraphic_Point b)
{
    return (CFBDGraphic_Vec2i) {(int32_t) a.x - (int32_t) b.x, (int32_t) a.y - (int32_t) b.y};
}
