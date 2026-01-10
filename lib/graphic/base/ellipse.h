/**
 * @file ellipse.h
 * @brief Ellipse drawing utilities for 2D graphics rendering.
 * @defgroup Ellipse_Module Ellipse Drawing
 * @ingroup Graphics_Module
 * @{
 */

#pragma once
#include "cfbd_graphic_define.h"
#include "point.h"

/**
 * @struct CCGraphic_Ellipse
 * @brief Represents an ellipse defined by center point and two radii.
 *
 * This structure defines an ellipse in 2D space using a center point and
 * separate radii for the X and Y axes. This allows for efficient rendering
 * of circles (when X_Radius == Y_Radius) and ellipses with arbitrary dimensions.
 *
 * @details
 * The ellipse is defined by:
 * - A center point (center)
 * - Horizontal radius extending from center along X-axis (X_Radius)
 * - Vertical radius extending from center along Y-axis (Y_Radius)
 *
 * The rendering is typically done using the Midpoint Ellipse Algorithm,
 * which provides accurate rasterization with good performance.
 *
 * @example
 * @code
 * // Create a circle with center at (160, 120) and radius 50
 * CCGraphic_Ellipse circle = {
 *     {160, 120},  // center
 *     50,          // X_Radius
 *     50           // Y_Radius
 * };
 * CFBDGraphic_DrawEllipse(device, &circle);
 *
 * // Create an ellipse with different horizontal and vertical radii
 * CCGraphic_Ellipse ellipse = {
 *     {160, 120},  // center
 *     80,          // X_Radius (wider)
 *     50           // Y_Radius (narrower)
 * };
 * CFBDGraphic_DrawEllipse(device, &ellipse);
 *
 * // Draw a filled ellipse
 * CCGraphic_Ellipse filled_ellipse = {{320, 240}, 30, 40};
 * CFBDGraphic_DrawFilledEllipse(device, &filled_ellipse);
 * @endcode
 */
typedef struct
{
    CFBDGraphic_Point center; /**< The center point of the ellipse */
    PointBaseType X_Radius;   /**< The horizontal radius (semi-major or semi-minor axis) */
    PointBaseType Y_Radius;   /**< The vertical radius (semi-minor or semi-major axis) */
} CCGraphic_Ellipse;

/**
 * @brief Renders the outline of an ellipse on the graphics device.
 *
 * This function draws the perimeter of an ellipse without filling the interior.
 * The ellipse is defined by its center point and horizontal and vertical radii.
 *
 * @param device   Pointer to the graphics device handler that performs the actual
 *                 rendering. Must be properly initialized before calling this function.
 * @param ellipse  Pointer to the ellipse structure containing the center point
 *                 and radius information.
 *
 * @return void
 *
 * @details
 * The function uses the Midpoint Ellipse Algorithm for accurate rasterization.
 * The algorithm ensures smooth, continuous lines with minimal computation overhead
 * suitable for embedded systems.
 *
 * @note
 * - Both X_Radius and Y_Radius should be greater than 0 for visible output.
 * - The radii are clamped to the valid coordinate range of the device.
 * - The line width is typically 1 pixel.
 *
 * @warning
 * - If @p device is NULL, undefined behavior occurs.
 * - If @p ellipse is NULL, undefined behavior occurs.
 * - Very large radii may exceed device boundaries.
 *
 * @example
 * @code
 * // Draw an outline circle with radius 50
 * CCGraphic_Ellipse circle = {{160, 120}, 50, 50};
 * CFBDGraphic_DrawEllipse(display_device, &circle);
 *
 * // Draw multiple ellipses
 * CCGraphic_Ellipse ellipses[] = {
 *     {{100, 100}, 30, 20},
 *     {{200, 100}, 40, 25},
 *     {{150, 200}, 50, 35}
 * };
 *
 * for (int i = 0; i < 3; i++) {
 *     CFBDGraphic_DrawEllipse(device, &ellipses[i]);
 * }
 * @endcode
 *
 * @see CCGraphic_Ellipse
 * @see CFBDGraphic_DrawFilledEllipse
 * @see CFBD_GraphicDevice
 */
void CFBDGraphic_DrawEllipse(CFBD_GraphicDevice* device, CCGraphic_Ellipse* ellipse);

/**
 * @brief Renders a filled ellipse on the graphics device.
 *
 * This function draws a completely filled ellipse using the specified graphic device.
 * All pixels within the elliptical region are rendered using the active color/pattern.
 *
 * @param device   Pointer to the graphics device handler. Must be initialized before use.
 * @param ellipse  Pointer to the ellipse structure defining the shape and position.
 *
 * @return void
 *
 * @details
 * The filled ellipse rendering process:
 * 1. Determines the bounding region of the ellipse
 * 2. For each scan line, calculates the horizontal extent of the ellipse
 * 3. Fills all pixels on that scan line within the ellipse boundary
 *
 * This is more efficient than drawing multiple outline ellipses.
 *
 * @note
 * - Uses the same mathematical approach as CFBDGraphic_DrawEllipse but fills interior.
 * - Performance scales linearly with the ellipse area.
 * - Clipping to device boundaries is handled automatically.
 *
 * @warning
 * - If @p device is NULL, undefined behavior occurs.
 * - If @p ellipse is NULL, undefined behavior occurs.
 * - Very large filled areas may have performance implications on slow devices.
 *
 * @example
 * @code
 * // Create a filled circle as a button
 * CCGraphic_Ellipse button = {{160, 120}, 40, 40};
 * CFBDGraphic_DrawFilledEllipse(device, &button);
 *
 * // Draw concentric filled ellipses for a target
 * for (int i = 3; i > 0; i--) {
 *     CCGraphic_Ellipse target_ring = {{320, 240}, i * 30, i * 30};
 *     CFBDGraphic_DrawFilledEllipse(device, &target_ring);
 * }
 *
 * // Create a filled ellipse overlay
 * CCGraphic_Ellipse overlay = {{160, 120}, 60, 40};
 * CFBDGraphic_DrawFilledEllipse(semi_transparent_device, &overlay);
 * @endcode
 *
 * @see CCGraphic_Ellipse
 * @see CFBDGraphic_DrawEllipse
 * @see CFBD_GraphicDevice
 */
void CFBDGraphic_DrawFilledEllipse(CFBD_GraphicDevice* device, CCGraphic_Ellipse* ellipse);

/** @} */ // End of Ellipse_Module group
