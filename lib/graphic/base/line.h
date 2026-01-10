/**
 * @file line.h
 * @brief Line drawing utilities for 2D graphics rendering.
 * @defgroup Line_Module Line Drawing
 * @ingroup Graphics_Module
 * @{
 */

#pragma once

#include "point.h"

/**
 * @struct CFBDGraphic_Line
 * @brief Represents a line segment defined by two endpoints.
 *
 * This structure defines a line in 2D space using two points. The line segment
 * extends from the left endpoint to the right endpoint, and can be used for
 * rendering, collision detection, and geometric calculations.
 *
 * @details
 * The naming convention "left" and "right" refers to the logical order of the
 * endpoints rather than spatial positioning. The endpoints can be at any position,
 * including when p_left is spatially to the right of p_right.
 *
 * @example
 * @code
 * // Create a horizontal line from (50, 100) to (200, 100)
 * CFBDGraphic_Line horizontal_line = {
 *     {50, 100},   // p_left
 *     {200, 100}   // p_right
 * };
 *
 * // Create a diagonal line from (0, 0) to (100, 100)
 * CFBDGraphic_Point start = {0, 0};
 * CFBDGraphic_Point end = {100, 100};
 * CFBDGraphic_Line diagonal = {start, end};
 *
 * // Using points defined elsewhere
 * CFBDGraphic_Line custom_line = {center_point, edge_point};
 * @endcode
 */
typedef struct
{
    CFBDGraphic_Point p_left;  /**< The left endpoint of the line segment */
    CFBDGraphic_Point p_right; /**< The right endpoint of the line segment */
} CFBDGraphic_Line;

/**
 * @brief Renders a line segment on the graphics device.
 *
 * This function draws a line from p_left to p_right on the specified graphics device.
 * The actual rendering algorithm and line appearance depend on the graphics device
 * implementation (e.g., OLED, LCD, framebuffer).
 *
 * @param handler Pointer to the graphics device handler structure that contains
 *                device-specific rendering functions and state information.
 * @param line    Pointer to the line segment to be drawn. The line defines the
 *                start and end points of the line segment.
 *
 * @return void
 *
 * @details
 * The function delegates the actual line drawing to the graphics device driver,
 * which may use Bresenham's algorithm, DDA algorithm, or other line rasterization
 * techniques depending on the device capabilities and performance requirements.
 *
 * @note
 * - The graphics handler must be properly initialized before calling this function.
 * - The line coordinates must be within the valid range of the graphics device.
 * - Clipping is performed by the device driver if necessary.
 *
 * @warning
 * - If @p handler is NULL, undefined behavior occurs.
 * - If @p line is NULL, undefined behavior occurs.
 * - The device must be initialized and ready for drawing.
 *
 * @example
 * @code
 * // Assuming device is properly initialized
 * CFBDGraphic_Line my_line = {{10, 10}, {100, 50}};
 * CFBDGraphic_DrawLine(graphics_device, &my_line);
 *
 * // Drawing multiple lines
 * CFBDGraphic_Line lines[] = {
 *     {{0, 0}, {100, 100}},
 *     {{0, 100}, {100, 0}},
 *     {{50, 0}, {50, 100}}
 * };
 *
 * for (int i = 0; i < 3; i++) {
 *     CFBDGraphic_DrawLine(graphics_device, &lines[i]);
 * }
 * @endcode
 *
 * @see CFBDGraphic_Line
 * @see CFBDGraphic_Point
 * @see CFBD_GraphicDevice
 */
void CFBDGraphic_DrawLine(CFBD_GraphicDevice* handler, CFBDGraphic_Line* line);

/** @} */ // End of Line_Module group
