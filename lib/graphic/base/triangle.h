/**
 * @file triangle.h
 * @brief Triangle drawing utilities for 2D graphics rendering.
 * @defgroup Triangle_Module Triangle Drawing
 * @ingroup Graphics_Module
 * @{
 */

#pragma once

#include "point.h"

/**
 * @struct CFBDGraphic_Triangle
 * @brief Represents a triangle defined by three vertices.
 *
 * This structure defines a triangle in 2D space using three corner points.
 * Triangles are fundamental geometric shapes used for creating complex graphics,
 * UI elements, and efficient polygon rendering through triangulation.
 *
 * @details
 * The triangle is defined by:
 * - @c p1: First vertex of the triangle
 * - @c p2: Second vertex of the triangle
 * - @c p3: Third vertex of the triangle
 *
 * The vertices can be specified in any order. For consistent rendering
 * and efficient backface culling, vertices are typically ordered counter-clockwise.
 *
 * Key properties:
 * - Vertices can form acute, obtuse, or right triangles
 * - Degenerate triangles (collinear points) result in no visible output
 * - Rendering may use barycentric interpolation for shading
 *
 * @example
 * @code
 * // Create a right triangle
 * CFBDGraphic_Triangle right_triangle = {
 *     {50, 50},    // p1: top-left
 *     {150, 50},   // p2: top-right
 *     {100, 150}   // p3: bottom-center
 * };
 * CCGraphic_DrawTriangle(device, &right_triangle);
 *
 * // Create an equilateral triangle
 * CFBDGraphic_Point center = {160, 120};
 * CFBDGraphic_Triangle equilateral = {
 *     {160, 50},   // top vertex
 *     {110, 170},  // bottom-left
 *     {210, 170}   // bottom-right
 * };
 * CCGraphic_DrawTriangle(device, &equilateral);
 *
 * // Create a thin triangle (like an arrow)
 * CFBDGraphic_Triangle arrow = {
 *     {160, 100},  // tip
 *     {140, 140},  // bottom-left
 *     {180, 140}   // bottom-right
 * };
 * CCGraphic_DrawFilledTriangle(device, &arrow);
 * @endcode
 */
typedef struct
{
    CFBDGraphic_Point p1; /**< First vertex of the triangle */
    CFBDGraphic_Point p2; /**< Second vertex of the triangle */
    CFBDGraphic_Point p3; /**< Third vertex of the triangle */
} CFBDGraphic_Triangle;

/**
 * @brief Renders the outline of a triangle on the graphics device.
 *
 * This function draws the three edges of a triangle without filling the interior.
 * The triangle is defined by connecting the three vertices with lines.
 *
 * @param handle   Pointer to the graphics device handler that performs the actual rendering.
 *                 Must be properly initialized before calling this function.
 * @param triangle Pointer to the triangle structure containing the three vertices.
 *
 * @return void
 *
 * @details
 * The rendering process:
 * 1. Draws a line from p1 to p2
 * 2. Draws a line from p2 to p3
 * 3. Draws a line from p3 to p1
 *
 * The three edges form a closed triangle outline.
 *
 * @note
 * - All three vertices should be distinct for visible output.
 * - Collinear vertices (forming a line) result in no visible triangle.
 * - The line width is typically 1 pixel.
 * - Drawing order may affect appearance with transparent or blended colors.
 *
 * @warning
 * - If @p handle is NULL, undefined behavior occurs.
 * - If @p triangle is NULL, undefined behavior occurs.
 * - Very large triangles may exceed device boundaries.
 *
 * @example
 * @code
 * // Draw a simple triangle outline
 * CFBDGraphic_Triangle tri = {
 *     {50, 50},
 *     {150, 50},
 *     {100, 150}
 * };
 * CCGraphic_DrawTriangle(device, &tri);
 *
 * // Draw a star pattern using multiple triangles
 * CFBDGraphic_Triangle star_triangles[] = {
 *     {{160, 80}, {140, 140}, {180, 140}},   // Point up
 *     {{160, 200}, {140, 140}, {180, 140}}   // Point down
 * };
 *
 * for (int i = 0; i < 2; i++) {
 *     CCGraphic_DrawTriangle(device, &star_triangles[i]);
 * }
 * @endcode
 *
 * @see CFBDGraphic_Triangle
 * @see CCGraphic_DrawFilledTriangle
 * @see CFBD_GraphicDevice
 */
void CCGraphic_DrawTriangle(CFBD_GraphicDevice* handle, CFBDGraphic_Triangle* triangle);

/**
 * @brief Renders a filled triangle on the graphics device.
 *
 * This function draws a completely filled triangle using the active color or pattern.
 * All pixels within the triangular region are rendered based on the scan-conversion algorithm.
 *
 * @param handle   Pointer to the graphics device handler for rendering.
 * @param triangle Pointer to the triangle structure containing the three vertices.
 *
 * @return void
 *
 * @details
 * Filled triangle rendering typically uses one of these algorithms:
 * - Barycentric coordinate method: Checks if each pixel is inside the triangle
 * - Edge function method: Uses cross products to determine point inclusion
 * - Scan line method: Fills horizontal spans within the triangle boundaries
 *
 * Performance characteristics:
 * - Scales with triangle area (number of pixels to fill)
 * - Efficient for UI elements and filled shapes
 * - Supports various fill colors and patterns depending on device
 *
 * @note
 * - Highly optimized for common triangle sizes (50-1000 pixels)
 * - Interior of triangle is fill, not anti-aliased edges
 * - Device driver handles clipping to screen boundaries
 *
 * @warning
 * - If @p handle is NULL, undefined behavior occurs.
 * - If @p triangle is NULL, undefined behavior occurs.
 * - Very large triangles may have performance implications
 * - Collinear vertices produce no output
 *
 * @example
 * @code
 * // Create a filled triangle shape
 * CFBDGraphic_Triangle filled_tri = {
 *     {160, 80},   // top
 *     {110, 180},  // bottom-left
 *     {210, 180}   // bottom-right
 * };
 * CCGraphic_DrawFilledTriangle(device, &filled_tri);
 *
 * // Draw layered triangles for gradient effect
 * for (int i = 0; i < 10; i++) {
 *     float scale = 0.9f - (i * 0.05f);
 *     CFBDGraphic_Triangle scaled_tri = {
 *         {160, (uint16_t)(80 + i * 5)},
 *         {(uint16_t)(110 - i * 3), (uint16_t)(180 - i * 5)},
 *         {(uint16_t)(210 + i * 3), (uint16_t)(180 - i * 5)}
 *     };
 *     CCGraphic_DrawFilledTriangle(device, &scaled_tri);
 * }
 *
 * // Draw arrow pointing downward
 * CFBDGraphic_Triangle arrow = {
 *     {160, 50},   // tip
 *     {130, 100},  // bottom-left
 *     {190, 100}   // bottom-right
 * };
 * CCGraphic_DrawFilledTriangle(device, &arrow);
 * @endcode
 *
 * @see CFBDGraphic_Triangle
 * @see CCGraphic_DrawTriangle
 * @see CFBD_GraphicDevice
 */
void CCGraphic_DrawFilledTriangle(CFBD_GraphicDevice* handle, CFBDGraphic_Triangle* triangle);

/** @} */ // End of Triangle_Module group