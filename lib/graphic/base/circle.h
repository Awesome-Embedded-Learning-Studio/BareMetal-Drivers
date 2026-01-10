/**
 * @file circle.h
 * @author Charliechen114514
 * @brief Circle geometry and drawing function declarations.
 * @defgroup Circle_Module Circle Drawing
 * @ingroup Graphics_Module
 * @{
 *
 * @page Circle_Guide Circle Drawing and Geometry Guide
 * @brief Comprehensive guide to drawing circles using the graphics library.
 *
 * ## Circle Fundamentals
 * Circles are defined by their center point and a radius. The BareMetal graphics
 * library provides efficient rendering functions for both circle outlines and
 * filled circles. These functions are optimized for embedded displays.
 *
 * ## Circle Structure
 * Each circle is represented by:
 * - A center point (CFBDGraphic_Point) specifying the circle's location
 * - A radius (SizeBaseType) in device coordinate units
 *
 * ## Rendering Algorithms
 * Implementations typically use the **Midpoint Circle Algorithm** (Bresenham's circle
 * algorithm) for efficient rasterization:
 * - Minimizes floating-point operations
 * - Generates anti-aliased appearance on pixel grids
 * - Scales well for circles of any size
 *
 * ## Common Use Cases
 * - UI buttons with circular shapes
 * - Radar displays and gauges
 * - Particle effects and animations
 * - Collision detection debugging
 * - Indicator lights and status displays
 *
 * @example
 * @code
 * // Example 1: Draw a simple circle outline
 * CFBDGraphicCircle indicator = {\n *     .radius = 20,
 *     .center = {160, 120}
 * };
 * CFBDGraphic_DrawCircle(display_device, &indicator);
 *
 * // Example 2: Draw filled circles of increasing size
 * for (int r = 10; r < 50; r += 10) {
 *     CFBDGraphicCircle circle = {.radius = r, .center = {160, 120}};
 *     CFBDGraphic_DrawFilledCircle(display_device, &circle);
 * }
 *
 * // Example 3: Multiple circles for a radar display
 * CFBDGraphicCircle radar_rings[] = {
 *     {.radius = 30, .center = {160, 120}},
 *     {.radius = 60, .center = {160, 120}},
 *     {.radius = 90, .center = {160, 120}}
 * };
 * for (int i = 0; i < 3; i++) {
 *     CFBDGraphic_DrawCircle(device, &radar_rings[i]);
 * }
 * @endcode
 *
 * @}
 */

#pragma once
#include "cfbd_graphic_define.h"
#include "point.h"

/**
 * @struct CFBDGraphicCircle
 * @ingroup Circle_Module
 * @brief Represents a circle defined by center point and radius.
 *
 * This structure describes a perfect circle in 2D screen space.
 * The circle is defined mathematically as the set of all points at
 * a fixed distance (radius) from the center point.
 *
 * @details
 * Circle rendering is performed using efficient integer-based algorithms
 * suitable for embedded displays. The radius should typically be in the
 * range 1-500 pixels for best performance on standard displays.
 *
 * @example
 * @code
 * // Create a circle with radius 50 centered at (160, 120)
 * CFBDGraphicCircle my_circle = {
 *     .radius = 50,
 *     .center = {160, 120}
 * };
 *
 * // Create multiple circles for concentric pattern
 * CFBDGraphicCircle circles[] = {
 *     {25, {160, 120}},
 *     {50, {160, 120}},
 *     {75, {160, 120}}
 * };
 * @endcode
 */
typedef struct
{
    /** @brief Radius of the circle in device units. */
    SizeBaseType radius;

    /** @brief Center coordinate of the circle. */
    CFBDGraphic_Point center;
} CFBDGraphicCircle;

/**
 * @brief Draw the outline of a circle on the graphics device.
 *
 * Renders only the perimeter of the circle without filling the interior.
 * The function uses the Midpoint Circle Algorithm for efficient rasterization.
 *
 * @param device Pointer to the target `CFBD_GraphicDevice`. Must be initialized.
 * @param circle Pointer to the `CFBDGraphicCircle` describing the circle to draw.
 *
 * @return void
 *
 * @details
 * The function:
 * - Computes circle edge pixels using integer-based arithmetic
 * - Handles clipping automatically to device bounds
 * - Uses 8-way symmetry for efficient rendering
 * - Produces smooth circular edges on pixel grids
 *
 * Performance is O(radius) for a circle outline.
 *
 * @note
 * - The device must be properly initialized before calling
 * - Radius should be > 0 for visible output
 * - Line width is typically 1 pixel
 *
 * @warning
 * - If device is NULL, undefined behavior occurs
 * - If circle is NULL, undefined behavior occurs
 *
 * @example
 * @code
 * // Draw a simple circle with radius 50
 * CFBDGraphicCircle circle = {50, {160, 120}};
 * CFBDGraphic_DrawCircle(my_device, &circle);
 *
 * // Draw concentric circles
 * for (int r = 20; r <= 80; r += 20) {
 *     CFBDGraphicCircle c = {r, {160, 120}};
 *     CFBDGraphic_DrawCircle(my_device, &c);
 * }
 *
 * // Draw a circle at multiple positions
 * CFBDGraphic_Point positions[] = {{80, 60}, {240, 60}, {160, 180}};
 * for (int i = 0; i < 3; i++) {
 *     CFBDGraphicCircle c = {30, positions[i]};
 *     CFBDGraphic_DrawCircle(my_device, &c);
 * }
 * @endcode
 *
 * @see CFBDGraphic_DrawFilledCircle
 * @see CFBDGraphicCircle
 */
void CFBDGraphic_DrawCircle(CFBD_GraphicDevice* device, CFBDGraphicCircle* circle);

/**
 * @brief Draw and fill a solid circle on the graphics device.
 *
 * Renders a completely filled circle with the active color.
 * All pixels within and on the circle boundary are rendered.
 *
 * @param device Pointer to the target `CFBD_GraphicDevice`. Must be initialized.
 * @param circle Pointer to the `CFBDGraphicCircle` describing the circle to fill.
 *
 * @return void
 *
 * @details
 * The filled circle implementation uses one of these approaches:
 * - **Scanline filling**: Renders horizontal spans across the circle
 * - **Incremental rasterization**: Fills based on distance calculations
 * - **Combined approach**: Uses symmetry to minimize computations
 *
 * Performance is O(radius²) as the entire area must be filled.
 *
 * @note
 * - The device must be initialized before calling
 * - Radius should be > 0 for visible output
 * - Clipping to device boundaries is automatic
 * - Suitable for UI buttons, gauges, and indicators
 *
 * @warning
 * - If device is NULL, undefined behavior occurs
 * - If circle is NULL, undefined behavior occurs
 * - Very large circles may impact performance
 *
 * @example
 * @code
 * // Draw a filled circle as a button
 * CFBDGraphicCircle button = {40, {160, 120}};
 * CFBDGraphic_DrawFilledCircle(my_device, &button);
 *
 * // Create a bulls-eye target pattern
 * for (int r = 50; r > 10; r -= 20) {
 *     CFBDGraphicCircle ring = {r, {160, 120}};
 *     CFBDGraphic_DrawFilledCircle(my_device, &ring);
 * }
 *
 * // Draw filled circles at different positions for particle effect
 * CFBDGraphic_Point particles[] = {{100, 80}, {220, 100}, {150, 200}};
 * for (int i = 0; i < 3; i++) {
 *     CFBDGraphicCircle particle = {5, particles[i]};
 *     CFBDGraphic_DrawFilledCircle(my_device, &particle);
 * }
 * @endcode
 *
 * @see CFBDGraphic_DrawCircle
 * @see CFBDGraphicCircle
 */
void CFBDGraphic_DrawFilledCircle(CFBD_GraphicDevice* device, CFBDGraphicCircle* circle);

/** @} */ // End of Circle_Module group
