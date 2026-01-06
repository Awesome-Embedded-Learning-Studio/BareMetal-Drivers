#include "ellipse.h"

#include "base_helpers.h"
#include "device/graphic_device.h"
#include "point.h"

#define PREANNOUNCE                                                                                \
    PointBaseType __x = 0;                                                                         \
    PointBaseType __y = 0

#define DRAW_OFFSET_POINT(offsetx, offsety)                                                        \
    do {                                                                                           \
        __x = ellipse->center.x + (offsetx);                                                       \
        __y = ellipse->center.y + (offsety);                                                       \
        setPixel(handler, __x, __y);                                                               \
    } while (0)

#define SQUARE(X) ((X) * (X))

static inline void clearBound(CFBD_GraphicDevice* handler,
                              CCGraphic_Ellipse* ellipse,
                              const int16_t x_radius,
                              const int16_t y_radius)
{
    int32_t lx = asInt32_t(ellipse->center.x) - x_radius;
    int32_t ty = asInt32_t(ellipse->center.y) - y_radius;
    int32_t rx = asInt32_t(ellipse->center.x) + x_radius;
    int32_t by = asInt32_t(ellipse->center.y) + y_radius;

    handler->ops->update_area(handler,
                              clamp_u16_from_i32(lx),
                              clamp_u16_from_i32(ty),
                              clamp_u16_from_i32(rx - lx + 1),
                              clamp_u16_from_i32(by - ty + 1));
}

void CFBDGraphic_DrawEllipse(CFBD_GraphicDevice* handler, CCGraphic_Ellipse* ellipse)
{
    PREANNOUNCE;
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            handler->ops->setPixel;
    const int16_t x_radius = ellipse->X_Radius;
    const int16_t y_radius = ellipse->Y_Radius;
    clearBound(handler, ellipse, x_radius, y_radius);
    // Bresenham's Ellipse Algorithm to avoid costly floating point calculations
    // Reference: https://blog.csdn.net/myf_666/article/details/128167392

    int16_t x = 0;
    int16_t y = y_radius;
    const int16_t y_radius_square = SQUARE(y_radius);
    const int16_t x_radius_square = SQUARE(x_radius);

    // Initial decision variable for the first region of the ellipse
    float d1 = y_radius_square + x_radius_square * (-y_radius + 0.5);

    // Draw initial points on the ellipse (4 points due to symmetry)
    DRAW_OFFSET_POINT(x, y);
    DRAW_OFFSET_POINT(-x, -y);
    DRAW_OFFSET_POINT(-x, y);
    DRAW_OFFSET_POINT(x, -y);

    // Draw the middle part of the ellipse (first region)
    while (y_radius_square * (x + 1) < x_radius_square * (y - 0.5)) {
        if (d1 <= 0) { // Next point is to the east of the current point
            d1 += y_radius_square * (2 * x + 3);
        }
        else { // Next point is southeast of the current point
            d1 += y_radius_square * (2 * x + 3) + x_radius_square * (-2 * y + 2);
            y--;
        }
        x++;

        // Draw ellipse arc for each point in the current region
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(x, -y);
    }

    // Draw the two sides of the ellipse (second region)
    float d2 = SQUARE(y_radius * (x + 0.5)) + SQUARE(x_radius * (y - 1)) -
               x_radius_square * y_radius_square;

    while (y > 0) {
        if (d2 <= 0) { // Next point is to the east of the current point
            d2 += y_radius_square * (2 * x + 2) + x_radius_square * (-2 * y + 3);
            x++;
        }
        else { // Next point is southeast of the current point
            d2 += x_radius_square * (-2 * y + 3);
        }
        y--;

        // Draw ellipse arc for each point on the sides
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(x, -y);
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(handler)) {
        int32_t lx = asInt32_t(ellipse->center.x) - x_radius;
        int32_t ty = asInt32_t(ellipse->center.y) - y_radius;
        int32_t rx = asInt32_t(ellipse->center.x) + x_radius;
        int32_t by = asInt32_t(ellipse->center.y) + y_radius;
        handler->ops->update_area(handler,
                                  clamp_u16_from_i32(lx),
                                  clamp_u16_from_i32(ty),
                                  clamp_u16_from_i32(rx - lx + 1),
                                  clamp_u16_from_i32(by - ty + 1));
    }
}

void CFBDGraphic_DrawFilledEllipse(CFBD_GraphicDevice* handler, CCGraphic_Ellipse* ellipse)
{
    PREANNOUNCE;
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            handler->ops->setPixel;
    const int16_t x_radius = ellipse->X_Radius;
    const int16_t y_radius = ellipse->Y_Radius;
    clearBound(handler, ellipse, x_radius, y_radius);
    // Bresenham's Ellipse Algorithm to avoid costly floating point calculations
    // Reference: https://blog.csdn.net/myf_666/article/details/128167392

    int16_t x = 0;
    int16_t y = y_radius;
    const int16_t y_radius_square = SQUARE(y_radius);
    const int16_t x_radius_square = SQUARE(x_radius);

    // Initial decision variable for the first region of the ellipse
    float d1 = y_radius_square + x_radius_square * (-y_radius + 0.5);
    // Fill the ellipse by drawing vertical lines in the specified range (filled area)
    for (int16_t j = -y; j < y; j++) {
        // Draw vertical lines to fill the area of the ellipse
        DRAW_OFFSET_POINT(0, j);
        DRAW_OFFSET_POINT(0, j);
    }

    // Draw initial points on the ellipse (4 points due to symmetry)
    DRAW_OFFSET_POINT(x, y);
    DRAW_OFFSET_POINT(-x, -y);
    DRAW_OFFSET_POINT(-x, y);
    DRAW_OFFSET_POINT(x, -y);

    // Draw the middle part of the ellipse (first region)
    while (y_radius_square * (x + 1) < x_radius_square * (y - 0.5)) {
        if (d1 <= 0) { // Next point is to the east of the current point
            d1 += y_radius_square * (2 * x + 3);
        }
        else { // Next point is southeast of the current point
            d1 += y_radius_square * (2 * x + 3) + x_radius_square * (-2 * y + 2);
            y--;
        }
        x++;

        // Fill the ellipse by drawing vertical lines in the current range
        for (int16_t j = -y; j < y; j++) {
            DRAW_OFFSET_POINT(x, j);
            DRAW_OFFSET_POINT(-x, j);
        }

        // Draw ellipse arc for each point in the current region
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(x, -y);
    }

    // Draw the two sides of the ellipse (second region)
    float d2 = SQUARE(y_radius * (x + 0.5)) + SQUARE(x_radius * (y - 1)) -
               x_radius_square * y_radius_square;

    while (y > 0) {
        if (d2 <= 0) { // Next point is to the east of the current point
            d2 += y_radius_square * (2 * x + 2) + x_radius_square * (-2 * y + 3);
            x++;
        }
        else { // Next point is southeast of the current point
            d2 += x_radius_square * (-2 * y + 3);
        }
        y--;

        // Fill the ellipse by drawing vertical lines in the current range
        for (int16_t j = -y; j < y; j++) {
            DRAW_OFFSET_POINT(x, j);
            DRAW_OFFSET_POINT(-x, j);
        }

        // Draw ellipse arc for each point on the sides
        DRAW_OFFSET_POINT(x, y);
        DRAW_OFFSET_POINT(-x, -y);
        DRAW_OFFSET_POINT(-x, y);
        DRAW_OFFSET_POINT(x, -y);
    }

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(handler)) {
        int32_t lx = asInt32_t(ellipse->center.x) - x_radius;
        int32_t ty = asInt32_t(ellipse->center.y) - y_radius;
        int32_t rx = asInt32_t(ellipse->center.x) + x_radius;
        int32_t by = asInt32_t(ellipse->center.y) + y_radius;

        handler->ops->update_area(handler,
                                  clamp_u16_from_i32(lx),
                                  clamp_u16_from_i32(ty),
                                  clamp_u16_from_i32(rx - lx + 1),
                                  clamp_u16_from_i32(by - ty + 1));
    }
}

#undef DRAW_OFFSET_POINT
#undef SQUARE