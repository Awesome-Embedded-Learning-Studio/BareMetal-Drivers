#include "circle.h"

#include <assert.h>

#include "base_helpers.h"
#include "device/graphic_device.h"
#include "point.h"

#define PREANNOUNCE                                                                                \
    PointBaseType __x = 0;                                                                         \
    PointBaseType __y = 0

#define DRAW_OFFSET_POINT(offsetx, offsety)                                                        \
    do {                                                                                           \
        __x = circle->center.x + (offsetx);                                                        \
        __y = circle->center.y + (offsety);                                                        \
        setPixel(handler, __x, __y);                                                               \
    } while (0)

static inline void
circle_calc_bbox(CFBDGraphicCircle* c, int32_t* lx, int32_t* ty, int32_t* rx, int32_t* by)
{
    *lx = asInt32_t(c->center.x) - c->radius;
    *ty = asInt32_t(c->center.y) - c->radius;
    *rx = asInt32_t(c->center.x) + c->radius;
    *by = asInt32_t(c->center.y) + c->radius;
}

static inline void clearBound(CFBD_GraphicDevice* handler, CFBDGraphicCircle* circle)
{
    int32_t lx, ty, rx, by;
    circle_calc_bbox(circle, &lx, &ty, &rx, &by);
    handler->ops->clear_area(handler,
                             clamp_u16_from_i32(lx),
                             clamp_u16_from_i32(ty),
                             clamp_u16_from_i32(rx - lx + 1),
                             clamp_u16_from_i32(by - ty + 1));
}

void CFBDGraphic_DrawCircle(CFBD_GraphicDevice* handler, CFBDGraphicCircle* circle)
{
    PREANNOUNCE;
    clearBound(handler, circle);
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            handler->ops->setPixel;
    /* Ref Doc: https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf*/
    /* Ref Toturial: https://www.bilibili.com/video/BV1VM4y1u7wJ*/
    int16_t d = 3 - circle->radius / 4;
    int16_t x = 0;
    int16_t y = circle->radius;

    DRAW_OFFSET_POINT(x, y);
    DRAW_OFFSET_POINT(-x, -y);
    DRAW_OFFSET_POINT(y, x);
    DRAW_OFFSET_POINT(-y, -x);

    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        }
        else {
            y--;
            d += 2 * (x - y) + 1;
        }
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(y, x);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-y, -x);
        DRAW_OFFSET_POINT(x, -y);
        DRAW_OFFSET_POINT(y, -x);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(-y, x);
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(handler)) {
        int32_t lx, ty, rx, by;
        circle_calc_bbox(circle, &lx, &ty, &rx, &by);
        handler->ops->update_area(handler,
                                  clamp_u16_from_i32(lx),
                                  clamp_u16_from_i32(ty),
                                  clamp_u16_from_i32(rx - lx + 1),
                                  clamp_u16_from_i32(by - ty + 1));
    }
}

void CFBDGraphic_DrawFilledCircle(CFBD_GraphicDevice* handler, CFBDGraphicCircle* circle)
{
    PREANNOUNCE;
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            handler->ops->setPixel;
    int16_t d = 1 - circle->radius;
    int16_t x = 0;
    int16_t y = circle->radius;
    clearBound(handler, circle);
    DRAW_OFFSET_POINT(x, y);
    DRAW_OFFSET_POINT(-x, -y);
    DRAW_OFFSET_POINT(y, x);
    DRAW_OFFSET_POINT(-y, -x);

    for (int16_t i = -y; i < y; i++)
        DRAW_OFFSET_POINT(0, i);

    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        }
        else {
            y--;
            d += 2 * (x - y) + 1;
        }
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(y, x);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-y, -x);
        DRAW_OFFSET_POINT(x, -y);
        DRAW_OFFSET_POINT(y, -x);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(-y, x);
        for (int16_t i = -y; i < y; i++) {
            DRAW_OFFSET_POINT(x, i);
            DRAW_OFFSET_POINT(-x, i);
        }
        for (int16_t i = -x; i < x; i++) {
            DRAW_OFFSET_POINT(y, i);
            DRAW_OFFSET_POINT(-y, i);
        }
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(handler)) {
        int32_t lx, ty, rx, by;
        circle_calc_bbox(circle, &lx, &ty, &rx, &by);
        handler->ops->update_area(handler,
                                  clamp_u16_from_i32(lx),
                                  clamp_u16_from_i32(ty),
                                  clamp_u16_from_i32(rx - lx + 1),
                                  clamp_u16_from_i32(by - ty + 1));
    }
}

#undef DRAW_OFFSET_POINT