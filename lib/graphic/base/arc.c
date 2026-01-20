#include "arc.h"

#include <assert.h>
#include <math.h>

#include "base_helpers.h"
#include "cfbd_define.h"
#include "cfbd_graphic_define.h"
#include "device/graphic_device.h"

static CFBD_Bool __pvt_is_in_angle(int16_t x, int16_t y, int16_t start, int16_t end)
{
    int16_t point_angle = (atan2(y, x) / 3.14 * 180);
    return start < end ? (start < point_angle && point_angle < end)
                       : (start > point_angle || point_angle > end);
}

static void clearArea(CFBD_GraphicDevice* device, CFBD_GraphicArc* arc)
{
    int32_t cx = asInt32_t(arc->center.x);
    int32_t cy = asInt32_t(arc->center.y);
    int32_t r = arc->radius;

    int32_t lx = cx - r;
    int32_t ty = cy - r;
    int32_t rx = cx + r;
    int32_t by = cy + r;

    uint16_t w = (uint16_t) (rx - lx + 1);
    uint16_t h = (uint16_t) (by - ty + 1);
    device->ops->clear_area(device, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty), w, h);
}

static void updateArea(CFBD_GraphicDevice* device, CFBD_GraphicArc* arc)
{
    int32_t cx = asInt32_t(arc->center.x);
    int32_t cy = asInt32_t(arc->center.y);
    int32_t r = arc->radius;

    int32_t lx = cx - r;
    int32_t ty = cy - r;
    int32_t rx = cx + r;
    int32_t by = cy + r;

    uint16_t w = (uint16_t) (rx - lx + 1);
    uint16_t h = (uint16_t) (by - ty + 1);
    device->ops->update_area(device, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty), w, h);
}

#define PREANNOUNCE                                                                                \
    PointBaseType __x = 0;                                                                         \
    PointBaseType __y = 0

#define DRAW_OFFSET_POINT(offsetx, offsety)                                                        \
    do {                                                                                           \
        __x = arc->center.x + (offsetx);                                                           \
        __y = arc->center.y + (offsety);                                                           \
        setPixel(device, __x, __y);                                                                \
    } while (0)

#define DRAW_IF_IN(offsetx, offsety)                                                               \
    do {                                                                                           \
        if (__pvt_is_in_angle((offsetx), (offsety), start_angle, end_angle)) {                     \
            DRAW_OFFSET_POINT(offsetx, offsety);                                                   \
        }                                                                                          \
    } while (0)

void CFBDGraphic_DrawArc(CFBD_GraphicDevice* device, CFBD_GraphicArc* arc)
{
    PREANNOUNCE;
    clearArea(device, arc);
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            device->ops->setPixel;
    /*此函数借用Bresenham算法画圆的方法*/
    int16_t x = 0;
    int16_t y = arc->radius;
    int16_t d = 1 - y;

    const int16_t start_angle = arc->start_degree;
    const int16_t end_angle = arc->end_degree;
    /*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
    DRAW_IF_IN(x, y);
    DRAW_IF_IN(-x, -y);
    DRAW_IF_IN(y, x);
    DRAW_IF_IN(-y, -x);

    while (x < y) // 遍历X轴的每个点
    {
        x++;
        if (d < 0) // 下一个点在当前点东方
        {
            d += 2 * x + 1;
        }
        else // 下一个点在当前点东南方
        {
            y--;
            d += 2 * (x - y) + 1;
        }

        /*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
        DRAW_IF_IN(x, y);
        DRAW_IF_IN(y, x);
        DRAW_IF_IN(-x, -y);
        DRAW_IF_IN(-y, -x);
        DRAW_IF_IN(x, -y);
        DRAW_IF_IN(y, -x);
        DRAW_IF_IN(-x, y);
        DRAW_IF_IN(-y, x);
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(device) && device->ops->update_area) {
        updateArea(device, arc);
    }
}

void CFBDGraphic_DrawFilledArc(CFBD_GraphicDevice* device, CFBD_GraphicArc* arc)
{
    PREANNOUNCE;
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            device->ops->setPixel;
    /*此函数借用Bresenham算法画圆的方法*/
    int16_t x = 0;
    int16_t y = arc->radius;
    int16_t d = 1 - y;
    clearArea(device, arc);
    const int16_t start_angle = arc->start_degree;
    const int16_t end_angle = arc->end_degree;
    __x = x;
    __y = y;

    /*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
    DRAW_IF_IN(x, y);
    DRAW_IF_IN(-x, -y);
    DRAW_IF_IN(y, x);
    DRAW_IF_IN(-y, -x);

    /*遍历起始点Y坐标*/
    for (int16_t j = -y; j < y; j++) {
        /*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
        DRAW_IF_IN(0, j);
    }

    while (x < y) // 遍历X轴的每个点
    {
        x++;
        if (d < 0) // 下一个点在当前点东方
        {
            d += 2 * x + 1;
        }
        else // 下一个点在当前点东南方
        {
            y--;
            d += 2 * (x - y) + 1;
        }

        /*在画圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
        DRAW_IF_IN(x, y);
        DRAW_IF_IN(y, x);
        DRAW_IF_IN(-x, -y);
        DRAW_IF_IN(-y, -x);
        DRAW_IF_IN(x, -y);
        DRAW_IF_IN(y, -x);
        DRAW_IF_IN(-x, y);
        DRAW_IF_IN(-y, x);

        /*遍历中间部分*/
        for (int16_t j = -y; j < y; j++) {
            /*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
            DRAW_IF_IN(x, j);
            DRAW_IF_IN(-x, j);
        }

        /*遍历两侧部分*/
        for (int16_t j = -x; j < x; j++) {
            /*在填充圆的每个点时，判断指定点是否在指定角度内，在，则画点，不在，则不做处理*/
            DRAW_IF_IN(y, j);
            DRAW_IF_IN(-y, j);
        }
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(device) && device->ops->update_area) {
        updateArea(device, arc);
    }
}

#undef DRAW_OFFSET_POINT
#undef DRAW_IF_IN
