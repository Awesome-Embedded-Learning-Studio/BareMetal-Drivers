/**
 * @file base_helpers.h
 * @brief Small numeric helpers for graphic base types.
 * @defgroup BaseHelpers_Module Graphic Base Helpers
 * @ingroup Graphics_Module
 * @{
 *
 * @page BaseHelpers_Guide Graphic Base Helpers User Guide
 * @brief Guide to using numeric conversion and clamping helpers.
 *
 * ## Overview
 * The BaseHelpers module provides lightweight inline conversion utilities
 * designed specifically for the BareMetal graphics system. These helpers
 * ensure safe arithmetic between different numeric types used throughout
 * the graphics library.
 *
 * ## Key Features
 * - Type-safe conversions between PointBaseType and 32-bit integers
 * - Automatic range clamping to prevent overflow
 * - Zero-cost abstraction suitable for embedded systems
 * - Inline implementation for optimal performance
 *
 * ## Usage Patterns
 * @code
 * // Converting unsigned 16-bit point coordinate to signed for calculations
 * CFBDGraphic_Point pt = {100, 50};
 * int32_t signed_x = asInt32_t(pt.x);  // 100 -> 100 (signed)
 * int32_t signed_y = asInt32_t(pt.y);  // 50 -> 50 (signed)
 *
 * // Clamping coordinates after arithmetic operations
 * int32_t new_x = 100 + 50000;  // Might overflow
 * PointBaseType safe_x = clamp_u16_from_i32(new_x);
 * // Result: UINT16_MAX (65535)
 *
 * // Handling negative displacements
 * int32_t delta = -50;
 * PointBaseType pos = clamp_u16_from_i32(delta);  // Result: 0
 * @endcode
 *
 * @see @ref Graphics_Module
 * @}
 */
#pragma once
#include <limits.h>
#include <stdint.h>

#include "cfbd_graphic_define.h"

/**
 * @ingroup BaseHelpers_Module
 * @brief Safely converts an unsigned 16-bit point coordinate to a signed 32-bit integer.
 *
 * This conversion is commonly used in intermediate calculations where negative
 * offsets or differences need to be represented and computed.
 *
 * @param v The unsigned 16-bit value (PointBaseType) to convert
 * @return int32_t The value as a signed 32-bit integer
 *
 * @example
 * @code
 * // Convert point coordinates for arithmetic operations
 * CFBDGraphic_Point p1 = {100, 100};
 * CFBDGraphic_Point p2 = {50, 75};
 *
 * // Calculate differences (may be negative)
 * int32_t dx = asInt32_t(p1.x) - asInt32_t(p2.x);  // 50
 * int32_t dy = asInt32_t(p1.y) - asInt32_t(p2.y);  // 25
 *
 * // Use for distance calculations
 * uint32_t distance = (dx * dx) + (dy * dy);
 * @endcode
 *
 * @note No range checking occurs during conversion; it's the caller's
 *       responsibility to handle potential negative values from subtraction.
 * @see clamp_u16_from_i32
 */
static inline int32_t asInt32_t(PointBaseType v)
{
    return (int32_t) v;
}

/**
 * @ingroup BaseHelpers_Module
 * @brief Clamps a signed 32-bit integer into the unsigned 16-bit range.
 *
 * This function safely constrains values to the valid PointBaseType (uint16_t) range,
 * mapping negative values to 0 and oversized values to UINT16_MAX. Essential for
 * coordinate arithmetic where intermediate calculations may exceed bounds.
 *
 * @param v The signed 32-bit value to clamp
 * @return PointBaseType The clamped value in the range [0, UINT16_MAX]
 *
 * @details
 * The clamping follows a three-step process:
 * 1. Negative values are mapped to 0
 * 2. Pathologically large values (> INT_MAX) are capped at INT_MAX
 * 3. Values exceeding UINT16_MAX are capped at UINT16_MAX
 *
 * This approach handles edge cases on platforms with unusual INT_MAX definitions.
 *
 * @example
 * @code
 * // Example 1: Positive value within range
 * PointBaseType result = clamp_u16_from_i32(100);
 * // result = 100
 *
 * // Example 2: Negative value (clamps to 0)
 * PointBaseType result = clamp_u16_from_i32(-50);
 * // result = 0
 *
 * // Example 3: Overflow case
 * PointBaseType result = clamp_u16_from_i32(100000);
 * // result = 65535 (UINT16_MAX)
 *
 * // Example 4: Coordinate offset calculation
 * int32_t x = 100, offset = 50000;
 * int32_t new_x = x + offset;           // 50100 (possibly clamped by device)
 * PointBaseType safe_x = clamp_u16_from_i32(new_x);  // 65535
 *
 * // Example 5: Safe point arithmetic
 * CFBDGraphic_Point origin = {50, 75};
 * int32_t shift_x = 100, shift_y = -80;
 * CFBDGraphic_Point new_point = {\n *     clamp_u16_from_i32((int32_t)origin.x + shift_x),
 *     clamp_u16_from_i32((int32_t)origin.y + shift_y)
 * };
 * // new_point = {150, 0}
 * @endcode
 *
 * @note This function uses saturation arithmetic, not wrapping arithmetic.
 *       Overflow silently clamps to UINT16_MAX rather than wrapping.
 * @see asInt32_t
 */
static inline PointBaseType clamp_u16_from_i32(int32_t v)
{
    /**
     * Clamp a signed 32-bit integer into the range representable by
     * `PointBaseType` (assumed to be an unsigned 16-bit type).
     *
     * This maps negative values to 0 and values larger than
     * `UINT16_MAX` to `UINT16_MAX`. An intermediate check against
     * `INT_MAX` is included to guard against pathological platforms
     * where `INT_MAX` is smaller than `UINT16_MAX`.
     */
    if (v < 0)
        return 0;
    if (v > INT_MAX)
        v = INT_MAX;
    if (v > UINT16_MAX)
        return UINT16_MAX;
    return (PointBaseType) v;
}

/** @} */ // End of BaseHelpers_Module group