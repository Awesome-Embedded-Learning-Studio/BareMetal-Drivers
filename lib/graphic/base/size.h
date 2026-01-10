/**
 * @file size.h
 * @brief Size data structure and utilities for 2D graphics.
 * @defgroup Size_Module Size Utilities
 * @ingroup Graphics_Module
 * @{
 */

#pragma once
#include "cfbd_graphic_define.h"
#include "device/graphic_device.h"

/**
 * @struct CFBDGraphicSize
 * @brief Represents dimensions (width and height) of a rectangular area.
 *
 * This structure defines the size of a 2D object using width and height components.
 * It is commonly used in conjunction with position information to define layout
 * and rendering areas for UI elements and graphics.
 *
 * @details
 * The size is defined by:
 * - @c width: The horizontal dimension in pixels
 * - @c height: The vertical dimension in pixels
 *
 * Both dimensions use SizeBaseType (typically uint16_t) for efficient storage
 * and fast computation on embedded systems.
 *
 * @example
 * @code
 * // Create a size for a 320x240 display
 * CFBDGraphicSize screen_size = {320, 240};
 *
 * // Create a size for a button
 * CFBDGraphicSize button_size = {80, 40};
 *
 * // Get screen size from device
 * CFBDGraphicSize dev_size;
 * CFBDGraphic_GetScreenSize(graphics_device, &dev_size);
 *
 * // Use size to calculate area
 * uint32_t area = dev_size.width * dev_size.height;
 * @endcode
 */
typedef struct
{
    SizeBaseType width;  /**< The width of the size in pixels */
    SizeBaseType height; /**< The height of the size in pixels */
} CFBDGraphicSize;

/**
 * @brief Retrieves the screen resolution from a graphics device.
 *
 * Queries the graphics device to obtain its display dimensions.
 * This function is essential for layout calculations and ensuring that
 * rendered content fits within the device's displayable area.
 *
 * @param device Pointer to the graphics device handler. Must be initialized and ready.
 * @param pSize  Pointer to CFBDGraphicSize structure where the screen dimensions
 *               will be stored.
 *
 * @return void
 *
 * @details
 * The function retrieves:
 * - width: The maximum horizontal coordinate (in pixels) that can be displayed
 * - height: The maximum vertical coordinate (in pixels) that can be displayed
 *
 * Typical values:
 * - Small OLED displays: 128x64, 128x128
 * - Standard displays: 320x240, 480x320
 * - Large displays: 800x600, 1024x768
 *
 * @note
 * - The device must be properly initialized before calling this function.
 * - The returned size is always non-zero for a functioning device.
 * - This function should be called before layout calculations.
 *
 * @warning
 * - If @p device is NULL, undefined behavior occurs.
 * - If @p pSize is NULL, undefined behavior occurs.
 * - Do not call before device initialization.
 *
 * @example
 * @code
 * // Get screen dimensions for layout calculations
 * CFBDGraphicSize screen_size;
 * CFBDGraphic_GetScreenSize(display_device, &screen_size);
 *
 * // Center an element on screen
 * uint16_t center_x = screen_size.width / 2;
 * uint16_t center_y = screen_size.height / 2;
 *
 * // Create a rectangle at screen center
 * CFBDGraphicRect element = {
 *     {center_x - 50, center_y - 25},
 *     {center_x + 50, center_y + 25}
 * };
 * CFBDGraphic_FillRect(device, &element);
 *
 * // Validate rendering bounds
 * if (object_width <= screen_size.width &&
 *     object_height <= screen_size.height) {
 *     // Object fits on screen, proceed with rendering
 * }
 * @endcode
 *
 * @see CFBDGraphicSize
 * @see CFBD_GraphicDevice
 */
void CFBDGraphic_GetScreenSize(CFBD_GraphicDevice* device, CFBDGraphicSize* pSize);

/** @} */ // End of Size_Module group
