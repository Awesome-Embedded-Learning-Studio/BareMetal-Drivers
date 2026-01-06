#include "line.h"

#include "base_helpers.h"
#include "cfbd_define.h"
#include "cfbd_graphic_define.h"
#include "device/graphic_device.h"
#include "point.h"

static inline void swap_int16(int16_t* val1, int16_t* val2)
{
    if (*val1 == *val2)
        return;
    *val1 ^= *val2;
    *val2 ^= *val1;
    *val1 ^= *val2;
}

static inline uint16_t max_uint16(uint16_t val1, uint16_t val2)
{
    return val1 > val2 ? val1 : val2;
}

static inline uint16_t min_uint16(uint16_t val1, uint16_t val2)
{
    return val1 < val2 ? val1 : val2;
}

static inline void clearBounds(CFBD_GraphicDevice* handler, CFBDGraphic_Line* line)
{
    int32_t lx = asInt32_t(line->p_left.x);
    int32_t rx = asInt32_t(line->p_right.x);
    int32_t ty = asInt32_t(line->p_left.y);
    int32_t by = asInt32_t(line->p_right.y);

    if (lx > rx) {
        int32_t t = lx;
        lx = rx;
        rx = t;
    }
    if (ty > by) {
        int32_t t = ty;
        ty = by;
        by = t;
    }
}

/*
    draw the lines that matches the equal x
*/
static void __on_handle_vertical_line(CFBD_GraphicDevice* handler, CFBDGraphic_Line* line)
{
    PointBaseType max_y = max_uint16(line->p_left.y, line->p_right.y);
    PointBaseType min_y = min_uint16(line->p_left.y, line->p_right.y);
    PointBaseType x = line->p_left.x;
    PointBaseType y;
    CFBD_Bool (*setpixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            handler->ops->setPixel;
    for (PointBaseType i = min_y; i <= max_y; i++) {
        y = i;
        setpixel(handler, x, y);
    }
}

static void __on_handle_horizental_line(CFBD_GraphicDevice* handler, CFBDGraphic_Line* line)
{
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            handler->ops->setPixel;
    PointBaseType max_x = max_uint16(line->p_left.x, line->p_right.x);
    PointBaseType min_x = min_uint16(line->p_left.x, line->p_right.x);
    PointBaseType x;
    PointBaseType y = line->p_left.y;
    for (PointBaseType i = min_x; i <= max_x; i++) {
        x = i;
        setPixel(handler, x, y);
    }
}

// Bresenham's Line Algorithm, designed to avoid floating point calculations
// References: https://www.cs.montana.edu/courses/spring2009/425/dslectures/Bresenham.pdf
// https://www.bilibili.com/video/BV1364y1d7Lo
void __pvt_BresenhamMethod_line(CFBD_GraphicDevice* handler, CFBDGraphic_Line* line)
{
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y) =
            handler->ops->setPixel;

#define __pvt_fast_draw_point(X, Y)                                                                \
    do {                                                                                           \
        setPixel(handler, X, Y);                                                                   \
    } while (0)

    // Define initial points for the line: p_left and p_right represent the endpoints
    int16_t startX = line->p_left.x;
    int16_t startY = line->p_left.y;
    int16_t endX = line->p_right.x;
    int16_t endY = line->p_right.y;

    // Flags to indicate transformations of coordinates
    uint8_t isYInverted = 0, isXYInverted = 0;
    {
        // If the start point's X coordinate is greater than the end point's X, swap the points
        if (startX > endX) {
            // Swap the X and Y coordinates for the start and end points
            swap_int16(&startX, &endX);
            swap_int16(&startY, &endY);
        }

        // If the start point's Y coordinate is greater than the end point's Y, invert the Y
        // coordinates
        if (startY > endY) {
            // Invert Y coordinates to make the line direction consistent in the first quadrant
            startY = -startY;
            endY = -endY;
            // Set the flag indicating Y coordinates were inverted
            isYInverted = 1;
        }

        // If the line's slope (dy/dx) is greater than 1, swap X and Y coordinates for a shallower
        // slope
        if (endY - startY > endX - startX) {
            // Swap X and Y coordinates for both points
            swap_int16(&startX, &startY);
            swap_int16(&endX, &endY);
            // Set the flag indicating both X and Y coordinates were swapped
            isXYInverted = 1;
        }

        // Calculate differences (dx, dy) and the decision variables for Bresenham's algorithm
        const int16_t dx = endX - startX;
        const int16_t dy = endY - startY;
        const int16_t incrE = 2 * dy;         // Increment for eastward movement
        const int16_t incrNE = 2 * (dy - dx); // Increment for northeastward movement

        int16_t decision = 2 * dy - dx; // Initial decision variable
        int16_t x = startX;             // Starting X coordinate
        int16_t y = startY;             // Starting Y coordinate

        // Draw the starting point and handle coordinate transformations based on flags
        if (isYInverted && isXYInverted) {
            __pvt_fast_draw_point(y, -x);
        }
        else if (isYInverted) {
            __pvt_fast_draw_point(x, -y);
        }
        else if (isXYInverted) {
            __pvt_fast_draw_point(y, x);
        }
        else {
            __pvt_fast_draw_point(x, y);
        }

        // Iterate through the X-axis to draw the rest of the line
        while (x < endX) {
            x++; // Increment X coordinate
            if (decision < 0) {
                decision += incrE; // Move eastward if decision variable is negative
            }
            else {
                y++; // Move northeastward if decision variable is positive or zero
                decision += incrNE;
            }

            // Draw each point along the line with coordinate transformation as needed
            if (isYInverted && isXYInverted) {
                __pvt_fast_draw_point(y, -x);
            }
            else if (isYInverted) {
                __pvt_fast_draw_point(x, -y);
            }
            else if (isXYInverted) {
                __pvt_fast_draw_point(y, x);
            }
            else {
                __pvt_fast_draw_point(x, y);
            }
        }
    }
}

void CFBDGraphic_DrawLine(CFBD_GraphicDevice* handler, CFBDGraphic_Line* line)
{
    clearBounds(handler, line);
    // test if the vertical
    if (line->p_left.x == line->p_right.x)
        __on_handle_vertical_line(handler, line);
    if (line->p_left.y == line->p_right.y)
        __on_handle_horizental_line(handler, line);
    __pvt_BresenhamMethod_line(handler, line);

    if (CFBDGraphic_DeviceRequestUpdateAtOnce(handler)) {
        int32_t lx = asInt32_t(line->p_left.x);
        int32_t rx = asInt32_t(line->p_right.x);
        int32_t ty = asInt32_t(line->p_left.y);
        int32_t by = asInt32_t(line->p_right.y);

        if (lx > rx) {
            int32_t t = lx;
            lx = rx;
            rx = t;
        }
        if (ty > by) {
            int32_t t = ty;
            ty = by;
            by = t;
        }

        handler->ops->update_area(handler,
                                  clamp_u16_from_i32(lx),
                                  clamp_u16_from_i32(ty),
                                  clamp_u16_from_i32(rx - lx + 1),
                                  clamp_u16_from_i32(by - ty + 1));
    }
}