/**
 * @file point.h
 * @brief Point and Vector data structures and utility functions for 2D graphics.
 * @defgroup Point_Module Point Utilities
 * @ingroup Graphics_Module
 * @{
 */

#pragma once
#include "cfbd_graphic_define.h"

/**
 * @struct CFBDGraphic_Point
 * @brief Represents a 2D point in the graphics coordinate system.
 *
 * This structure defines a point in 2D space with unsigned 16-bit coordinates.
 * It is used throughout the graphics module for positioning and drawing operations.
 *
 * @details
 * The coordinates are limited to the range [0, UINT16_MAX] by the PointBaseType definition.
 * This allows for efficient storage and computation on embedded systems.
 *
 * @example
 * @code
 * // Create a point at coordinates (100, 200)
 * CFBDGraphic_Point my_point = {100, 200};
 *
 * // Add two points together with automatic clamping
 * CFBDGraphic_Point point1 = {50, 75};
 * CFBDGraphic_Point point2 = {100, 150};
 * CFBDGraphic_Point result = point_add(point1, point2);
 * // result: {150, 225}
 * @endcode
 */
typedef struct
{
    PointBaseType x; /**< X-coordinate of the point */
    PointBaseType y; /**< Y-coordinate of the point */
} CFBDGraphic_Point;

/**
 * @struct CFBDGraphic_Vec2i
 * @brief Represents a 2D vector with signed 32-bit integer components.
 *
 * This structure is used for vector operations that may result in negative values,
 * such as the difference between two points. The use of 32-bit signed integers
 * allows for proper representation of negative displacements.
 *
 * @example
 * @code
 * // Calculate the vector from point1 to point2
 * CFBDGraphic_Point point1 = {100, 100};
 * CFBDGraphic_Point point2 = {150, 75};
 * CFBDGraphic_Vec2i displacement = point_sub(point2, point1);
 * // displacement: {50, -25}
 * @endcode
 */
typedef struct
{
    int32_t x; /**< X-component of the vector */
    int32_t y; /**< Y-component of the vector */
} CFBDGraphic_Vec2i;

/**
 * @brief Adds two points together with automatic saturation.
 *
 * This function performs component-wise addition of two points. If the result
 * exceeds UINT16_MAX, the coordinate is clamped to UINT16_MAX to prevent overflow.
 *
 * @param a The first point operand
 * @param b The second point operand
 * @return CFBDGraphic_Point The sum of the two points with saturation applied
 *
 * @details
 * This function ensures that the resulting point remains within the valid coordinate
 * range of the graphics system. The saturation behavior prevents wraparound and
 * maintains visual correctness.
 *
 * @warning Saturation occurs silently without notification. If exact values are
 * critical, pre-check coordinates before addition.
 *
 * @example
 * @code
 * CFBDGraphic_Point p1 = {100, 50};
 * CFBDGraphic_Point p2 = {200, 150};
 * CFBDGraphic_Point sum = point_add(p1, p2);
 * // sum = {300, 200}
 *
 * // Saturation example
 * CFBDGraphic_Point p3 = {65500, 65535};
 * CFBDGraphic_Point p4 = {100, 100};
 * CFBDGraphic_Point saturated = point_add(p3, p4);
 * // saturated = {65535, 65535} (clamped)
 * @endcode
 */
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

/**
 * @brief Computes the difference vector between two points.
 *
 * This function calculates the displacement vector from point @p b to point @p a.
 * The result is a signed vector that can represent displacements in all directions.
 *
 * @param a The destination point (minuend)
 * @param b The source point (subtrahend)
 * @return CFBDGraphic_Vec2i The displacement vector from @p b to @p a
 *
 * @details
 * The function performs component-wise subtraction using 32-bit signed arithmetic
 * to handle all possible displacement values without overflow issues that could
 * occur with unsigned arithmetic.
 *
 * @example
 * @code
 * CFBDGraphic_Point center = {320, 240};
 * CFBDGraphic_Point point = {400, 200};
 *
 * // Calculate displacement from center to point
 * CFBDGraphic_Vec2i offset = point_sub(point, center);
 * // offset = {80, -40}
 *
 * // Negative displacement example
 * CFBDGraphic_Point p1 = {50, 100};
 * CFBDGraphic_Point p2 = {200, 300};
 * CFBDGraphic_Vec2i vec = point_sub(p1, p2);
 * // vec = {-150, -200}
 * @endcode
 */
static inline CFBDGraphic_Vec2i point_sub(CFBDGraphic_Point a, CFBDGraphic_Point b)
{
    return (CFBDGraphic_Vec2i) {(int32_t) a.x - (int32_t) b.x, (int32_t) a.y - (int32_t) b.y};
}

/** @} */ // End of Point_Module group
