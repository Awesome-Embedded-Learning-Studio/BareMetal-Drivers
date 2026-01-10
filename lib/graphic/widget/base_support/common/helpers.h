/**
 * @file helpers.h
 * @brief Utility helper macros and functions for graphics operations.
 * @ingroup Graphics_Helpers
 *
 * @details
 * This module provides commonly used utility macros and inline functions
 * for graphics programming, including min/max operations, clamping, and
 * other utility functions used throughout the graphics framework.
 *
 * @see CFBD_BaseAnimation for animation helpers
 * @see graphic_device.h for graphics device helpers
 */

#pragma once
#include <stdint.h>

/**
 * @defgroup Graphics_Helpers Graphics Utility Helpers
 * @ingroup Graphics
 * @brief Utility macros and functions for graphics operations.
 *
 * @details
 * This module provides commonly used utility macros and inline functions
 * to simplify graphics programming and reduce code duplication. These
 * utilities cover common operations like value comparison, clamping, and
 * boundary checking.
 *
 * Includes:
 * - MIN/MAX macros for value comparison
 * - Clamping function for range constraint
 * - Additional graphics-specific helpers
 *
 * @{\n */

/**
 * @def MAX
 * @brief Return the maximum of two values.
 *
 * @details
 * Evaluates to the greater of the two arguments. Arguments are evaluated
 * only once when possible.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 * @return The maximum value.
 *
 * @note Only defined if not already defined elsewhere.
 *
 * @example
 * @code
 * int larger = MAX(100, 200);  // Result: 200
 * uint16_t width = MAX(device_width, min_width);
 * @endcode
 */
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

/**
 * @def MIN
 * @brief Return the minimum of two values.
 *
 * @details
 * Evaluates to the lesser of the two arguments. Arguments are evaluated
 * only once when possible.
 *
 * @param a First value to compare.
 * @param b Second value to compare.
 * @return The minimum value.
 *
 * @note Only defined if not already defined elsewhere.
 *
 * @example
 * @code
 * int smaller = MIN(100, 200);  // Result: 100
 * uint16_t height = MIN(device_height, max_height);
 * @endcode
 */
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/**
 * @brief Clamp a signed 32-bit integer to a specified range.
 *
 * @details
 * Constrains the input value to be within the specified lower and upper
 * bounds. If the value is below the lower bound, returns the lower bound.
 * If above the upper bound, returns the upper bound. Otherwise, returns
 * the original value unchanged.
 *
 * This is commonly used for:
 * - Constraining coordinates to display boundaries
 * - Limiting values to valid ranges
 * - Clipping values before computation
 *
 * @param v The value to clamp.
 * @param lo The lower bound (inclusive).
 * @param hi The upper bound (inclusive).
 *
 * @return The clamped value: min(max(v, lo), hi)
 *
 * @note Has no side effects; safe for use with function calls as arguments.
 *
 * @example
 * @code
 * // Clamp coordinate to display range
 * int32_t x = clamp_i32(user_input_x, 0, 127);  // Range [0, 127]
 *
 * // Clamp position within valid range
 * int32_t y = clamp_i32(y_offset, -50, 50);  // Range [-50, 50]
 *
 * // Clamp in animation loop
 * int32_t frame = clamp_i32(current_frame, 0, max_frames - 1);
 * @endcode
 *
 * @see MAX and MIN macros for simple comparison operations
 */
static inline int32_t clamp_i32(int32_t v, int32_t lo, int32_t hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

/** @} */ // end of Graphics_Helpers group