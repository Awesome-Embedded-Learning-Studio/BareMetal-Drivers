#include "triangle.h"

#include "base_helpers.h"
#include "cfbd_graphic_define.h"
#include "device/graphic_device.h"
#include "line.h"
#include "point.h"

static inline int16_t find_int16min(int16_t* vals, int16_t size)
{
    int16_t result = vals[0];
    for (int16_t i = 0; i < size; i++) {
        if (vals[i] < result)
            result = vals[i];
    }
    return result;
}

static inline int16_t find_int16max(int16_t* vals, int16_t size)
{
    int16_t result = vals[0];
    for (int16_t i = 0; i < size; i++) {
        if (vals[i] > result)
            result = vals[i];
    }
    return result;
}

static void clearBound(CFBD_GraphicDevice* handle, CFBDGraphic_Triangle* triangle)
{
    int32_t lx = asInt32_t(triangle->p1.x);
    int32_t ty = asInt32_t(triangle->p1.y);
    int32_t rx = lx;
    int32_t by = ty;

    /* p2 */
    if (asInt32_t(triangle->p2.x) < lx)
        lx = asInt32_t(triangle->p2.x);
    if (asInt32_t(triangle->p2.y) < ty)
        ty = asInt32_t(triangle->p2.y);
    if (asInt32_t(triangle->p2.x) > rx)
        rx = asInt32_t(triangle->p2.x);
    if (asInt32_t(triangle->p2.y) > by)
        by = asInt32_t(triangle->p2.y);

    /* p3 */
    if (asInt32_t(triangle->p3.x) < lx)
        lx = asInt32_t(triangle->p3.x);
    if (asInt32_t(triangle->p3.y) < ty)
        ty = asInt32_t(triangle->p3.y);
    if (asInt32_t(triangle->p3.x) > rx)
        rx = asInt32_t(triangle->p3.x);
    if (asInt32_t(triangle->p3.y) > by)
        by = asInt32_t(triangle->p3.y);

    /* 防御式处理 */
    if (rx < lx)
        rx = lx;
    if (by < ty)
        by = ty;

    uint16_t w = (uint16_t) (rx - lx + 1);
    uint16_t h = (uint16_t) (by - ty + 1);
    handle->ops->clear_area(handle, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty), w, h);
}

static void update_requests(CFBD_GraphicDevice* handle, CFBDGraphic_Triangle* triangle)
{
    int32_t lx = asInt32_t(triangle->p1.x);
    int32_t ty = asInt32_t(triangle->p1.y);
    int32_t rx = lx;
    int32_t by = ty;

    /* p2 */
    if (asInt32_t(triangle->p2.x) < lx)
        lx = asInt32_t(triangle->p2.x);
    if (asInt32_t(triangle->p2.y) < ty)
        ty = asInt32_t(triangle->p2.y);
    if (asInt32_t(triangle->p2.x) > rx)
        rx = asInt32_t(triangle->p2.x);
    if (asInt32_t(triangle->p2.y) > by)
        by = asInt32_t(triangle->p2.y);

    /* p3 */
    if (asInt32_t(triangle->p3.x) < lx)
        lx = asInt32_t(triangle->p3.x);
    if (asInt32_t(triangle->p3.y) < ty)
        ty = asInt32_t(triangle->p3.y);
    if (asInt32_t(triangle->p3.x) > rx)
        rx = asInt32_t(triangle->p3.x);
    if (asInt32_t(triangle->p3.y) > by)
        by = asInt32_t(triangle->p3.y);

    /* 防御式处理 */
    if (rx < lx)
        rx = lx;
    if (by < ty)
        by = ty;

    uint16_t w = (uint16_t) (rx - lx + 1);
    uint16_t h = (uint16_t) (by - ty + 1);
    handle->ops->update_area(handle, clamp_u16_from_i32(lx), clamp_u16_from_i32(ty), w, h);
}

void CCGraphic_DrawTriangle(CFBD_GraphicDevice* handle, CFBDGraphic_Triangle* triangle)
{
    clearBound(handle, triangle);
    CFBDGraphic_Line line;
    line.p_left = triangle->p1;
    line.p_right = triangle->p2;
    CFBDGraphic_DrawLine(handle, &line);
    line.p_left = triangle->p2;
    line.p_right = triangle->p3;
    CFBDGraphic_DrawLine(handle, &line);
    line.p_left = triangle->p1;
    CFBDGraphic_DrawLine(handle, &line);

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(handle)) {
        update_requests(handle, triangle);
    }
}

static uint8_t
__pvt_is_in_triangle(int16_t* triangles_x, int16_t* triangles_y, PointBaseType x, PointBaseType y)
{
    uint8_t is_in = 0;
    /* https://wrfranklin.org/Research/Short_Notes/pnpoly.html */
    for (uint8_t i = 0, j = 2; i < 3; j = i++) {
        if (((triangles_y[i] > y) != (triangles_y[j] > y)) &&
            (x < (triangles_x[j] - triangles_x[i]) * (y - triangles_y[i]) /
                                 (triangles_y[j] - triangles_y[i]) +
                         triangles_x[i])) {
            is_in = !is_in;
        }
    }
    return is_in;
}

void CCGraphic_DrawFilledTriangle(CFBD_GraphicDevice* handle, CFBDGraphic_Triangle* triangle)
{
    clearBound(handle, triangle);
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            handle->ops->setPixel;

    int16_t triangles_x[] = {triangle->p1.x, triangle->p2.x, triangle->p3.x};

    int16_t triangles_y[] = {triangle->p1.y, triangle->p2.y, triangle->p3.y};

    int16_t minX = find_int16min(triangles_x, 3);
    int16_t minY = find_int16min(triangles_y, 3);

    int16_t maxX = find_int16max(triangles_x, 3);
    int16_t maxY = find_int16max(triangles_y, 3);

    for (int16_t i = minX; i < maxX; i++) {
        for (int16_t j = minY; j < maxY; j++) {
            if (__pvt_is_in_triangle(triangles_x, triangles_y, i, j)) {
                setPixel(handle, i, j);
            }
        }
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(handle)) {
        update_requests(handle, triangle);
    }
}
