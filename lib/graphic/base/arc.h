/**
 * @file arc.h
 * @author Charliechen114514
 * @brief Arc geometry and drawing prototypes for the graphics subsystem.
 * @defgroup Arc_Module Arc Drawing
 * @ingroup Graphics_Module
 * @{
 *
 * @page Arc_Guide Arc Drawing and Geometry Guide
 * @brief Comprehensive guide to arc drawing and sector rendering.
 *
 * ## Arc Fundamentals
 * Arcs represent portions of a circle defined by:
 * - Center point and radius (like circles)
 * - Angular range (start and end degrees)
 * - Direction of sweep (typically counter-clockwise)
 *
 * ## Angular Measurement
 * Angles are specified in **degrees (0-359)**:
 * - 0° points to the right (3 o'clock position)
 * - 90° points downward (6 o'clock position)
 * - 180° points to the left (9 o'clock position)
 * - 270° points upward (12 o'clock position)
 * - Positive angles sweep counter-clockwise
 * - Angles wrap at 360° boundaries
 *
 * ## Arc Types
 * 1. **Arc outline**: Just the curved edge (CFBDGraphic_DrawArc)
 * 2. **Filled arc/sector**: Arc plus two radii forming a pie slice (CFBDGraphic_DrawFilledArc)
 *
 * ## Practical Applications
 * - Gauges and progress indicators
 * - Pie charts and donut charts
 * - Circular menus and dial controls
 * - Clock and timer displays
 * - Compass and rotation indicators
 * - Loading animations
 *
 * @example
 * @code
 * // Example 1: 90-degree quarter arc
 * CFBD_GraphicArc quarter = {
 *     .center = {160, 120},
 *     .radius = 50,
 *     .start_degree = 0,
 *     .end_degree = 90
 * };
 * CFBDGraphic_DrawArc(device, &quarter);
 *
 * // Example 2: Progress indicator (filled arc)
 * CFBD_GraphicArc progress = {
 *     .center = {160, 120},
 *     .radius = 40,
 *     .start_degree = 0,
 *     .end_degree = 270  // 75% complete
 * };
 * CFBDGraphic_DrawFilledArc(device, &progress);
 *
 * // Example 3: Clock hour markers (12 arcs)
 * for (int i = 0; i < 12; i++) {
 *     int angle = (i * 30);  // 30 degrees per hour
 *     CFBD_GraphicArc marker = {
 *         .center = {160, 120},
 *         .radius = 60,
 *         .start_degree = angle,
 *         .end_degree = angle + 5
 *     };
 *     CFBDGraphic_DrawArc(device, &marker);
 * }
 * @endcode
 *
 * @}
 */
#pragma once

#include "point.h"

/**
 * @struct CFBD_GraphicArc
 * @ingroup Arc_Module
 * @brief Represents a circular arc defined by center, radius, and angular range.
 *
 * An arc is a portion of a circle specified by a center point, radius,
 * and a pair of angles defining the start and end of the arc.
 *
 * @details
 * Arc definition:
 * - **center**: The center point of the circle from which the arc is drawn
 * - **radius**: Distance from center to the arc edge, in device units
 * - **start_degree**: Starting angle in degrees (0-359)
 * - **end_degree**: Ending angle in degrees (0-359)
 *
 * Angular convention:
 * - 0° points horizontally right (3 o'clock)
 * - Positive angles sweep counter-clockwise
 * - Angles are automatically normalized to [0, 359]
 * - If start_degree > end_degree, the arc wraps around 0°
 *
 * @example
 * @code
 * // Create a 90-degree arc in the first quadrant
 * CFBD_GraphicArc quarter_arc = {
 *     .center = {160, 120},
 *     .radius = 50,
 *     .start_degree = 0,
 *     .end_degree = 90
 * };
 *
 * // Create a semicircle (180 degrees)
 * CFBD_GraphicArc semi = {
 *     .center = {160, 120},
 *     .radius = 50,
 *     .start_degree = 0,
 *     .end_degree = 180
 * };
 *
 * // Create an arc that wraps (e.g., from 270° to 90° = 180° arc)
 * CFBD_GraphicArc wrap_arc = {
 *     .center = {160, 120},
 *     .radius = 50,
 *     .start_degree = 270,
 *     .end_degree = 90  // Wraps through 0°
 * };
 * @endcode
 */
typedef struct __CFBD_GraphicArc
{
    /** @brief Center point of the arc's parent circle. */
    CFBDGraphic_Point center;

    /** @brief Radius from center to the arc in device units. */
    PointBaseType radius;

    /** @brief Starting angle in degrees (0-359), measured counter-clockwise from right (3 o'clock).
     */
    int16_t start_degree;

    /** @brief Ending angle in degrees (0-359). The arc sweeps from start_degree to end_degree. */
    int16_t end_degree;
} CFBD_GraphicArc;

/**
 * @brief Draw the outline of an arc on the provided graphic device.
 *
 * Renders only the curved edge of the arc segment without filling the interior
 * or drawing radii. The arc is defined by its center, radius, and angular range.
 *
 * @param device Target graphic device pointer. Must be a valid and initialized device.
 * @param circle Pointer to a `CFBD_GraphicArc` describing the arc to draw.
 *
 * @return void
 *
 * @details
 * The arc is drawn as a series of pixels forming the curved edge between
 * start_degree and end_degree. The implementation:
 * - Uses angular subdivision or Bresenham-style arc rasterization
 * - Handles device clipping automatically
 * - Maintains smooth appearance across the angular range
 * - Operates non-blocking for embedded systems
 *
 * Performance is O(radius * angle_range).\n *
 * @note
 * - All angles are in degrees (0-359)
 * - 0° is at 3 o'clock, angles increase counter-clockwise
 * - Line width is typically 1 pixel
 * - Device must be initialized before calling
 *
 * @warning
 * - If device is NULL, undefined behavior occurs
 * - If arc is NULL, undefined behavior occurs
 *
 * @example
 * @code
 * // Draw a 90-degree quarter arc
 * CFBD_GraphicArc quarter = {
 *     .center = {160, 120},
 *     .radius = 50,
 *     .start_degree = 0,
 *     .end_degree = 90
 * };
 * CFBDGraphic_DrawArc(device, &quarter);
 *
 * // Draw multiple arcs for a speedometer
 * for (int angle = 0; angle < 180; angle += 30) {
 *     CFBD_GraphicArc tick = {
 *         .center = {160, 120},
 *         .radius = 60,
 *         .start_degree = angle,
 *         .end_degree = angle + 5
 *     };
 *     CFBDGraphic_DrawArc(device, &tick);
 * }
 *
 * // Draw wrapping arc (e.g., from 330° to 30°)
 * CFBD_GraphicArc wrap = {
 *     .center = {160, 120},
 *     .radius = 45,
 *     .start_degree = 330,
 *     .end_degree = 30  // Wraps through 0°
 * };
 * CFBDGraphic_DrawArc(device, &wrap);
 * @endcode
 *
 * @see CFBDGraphic_DrawFilledArc
 * @see CFBD_GraphicArc
 */
void CFBDGraphic_DrawArc(CFBD_GraphicDevice* device, CFBD_GraphicArc* circle);

/**
 * @brief Draw a filled arc (sector) on the provided graphic device.
 *
 * Renders a complete filled sector: the curved arc plus the two radii connecting
 * to the center, forming a pie-slice or wedge shape.
 *
 * @param device Target graphic device pointer. Must be initialized and valid.
 * @param circle Pointer to a `CFBD_GraphicArc` describing the sector to fill.
 *
 * @return void
 *
 * @details
 * A filled arc combines three elements:
 * 1. The outer curved arc edge
 * 2. Left radius from center to arc at start_degree
 * 3. Right radius from center to arc at end_degree
 * 4. All pixels between these boundaries are filled
 *
 * The filling algorithm:
 * - Iterates through the angular range
 * - For each angle, fills radially from center to arc edge
 * - Automatically handles device clipping
 * - Optimized for embedded rasterization
 *
 * Performance is O(radius² * angle_range / 360).\n *
 * @note
 * - Angles measured in degrees (0-359)
 * - 0° is at 3 o'clock, counter-clockwise convention
 * - Device must be initialized before calling
 * - Suitable for progress indicators, pie charts, gauge fills
 *
 * @warning
 * - If device is NULL, undefined behavior occurs
 * - If arc is NULL, undefined behavior occurs
 * - Very large arcs or small devices may have performance impact
 *
 * @example
n * @code
 * // Draw a progress indicator at 75%
 * CFBD_GraphicArc progress = {
 *     .center = {160, 120},
 *     .radius = 40,
 *     .start_degree = 0,
 *     .end_degree = 270  // 75% of 360
 * };
 * CFBDGraphic_DrawFilledArc(device, &progress);
 *
 * // Draw pie chart segments
 * CFBD_GraphicArc segments[] = {
 *     {.center = {160, 120}, .radius = 50, .start_degree = 0,   .end_degree = 120},
 *     {.center = {160, 120}, .radius = 50, .start_degree = 120, .end_degree = 240},
 *     {.center = {160, 120}, .radius = 50, .start_degree = 240, .end_degree = 360}
 * };
 * for (int i = 0; i < 3; i++) {
 *     CFBDGraphic_DrawFilledArc(device, &segments[i]);
 * }
 *
 * // Draw loading spinner segment (30 degrees)
 * CFBD_GraphicArc spinner = {
 *     .center = {160, 120},
 *     .radius = 50,
 *     .start_degree = 45,
 *     .end_degree = 75  // 30-degree filled sector
 * };
 * CFBDGraphic_DrawFilledArc(device, &spinner);
 * @endcode
 *
 * @see CFBDGraphic_DrawArc
 * @see CFBD_GraphicArc
 */
void CFBDGraphic_DrawFilledArc(CFBD_GraphicDevice* device, CFBD_GraphicArc* circle);

/** @} */ // End of Arc_Module group
