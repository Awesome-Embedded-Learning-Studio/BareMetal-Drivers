/**
 * @file image.h
 * @brief Image widget for rendering bitmap graphics
 * @details Provides image structures and functions for drawing bitmap data
 *          at specified positions on graphics devices. Supports arbitrary
 *          bitmap data with configurable size and position.
 * @ingroup Graphics_Widget Graphics_Image
 * @{
 */

#pragma once
#include "base/point.h"
#include "base/rectangle.h"
#include "base/size.h"
#include "cfbd_graphic_define.h"

/**
 * @typedef CCGraphic_Image
 * @brief Image widget structure containing bitmap data and position
 * @details Represents a bitmap image that can be drawn on a graphics device.
 *          The image data is referenced via sources_register pointer, allowing
 *          for flexible memory management strategies.
 * @see CFBDGraphic_DrawImage
 */
typedef struct
{
    /** @brief Top-left position of image in pixels */
    CFBDGraphic_Point point;

    /** @brief Dimensions of image in pixels */
    CFBDGraphicSize image_size;

    /** @brief Pointer to bitmap data (owned by caller) */
    uint8_t* sources_register;
} CCGraphic_Image;

/**
 * @brief Initialize an image widget structure
 * @details Sets up an image widget with position, size, and bitmap data.
 *          All pointer parameters must remain valid for the lifetime of
 *          the image structure usage.
 * @param[out] image - Image structure to initialize
 * @param[in] tl_point - Top-left position in pixels
 * @param[in] image_size - Width and height dimensions
 * @param[in] sources_register - Pointer to bitmap data
 * @note The caller retains ownership of all memory (point, size, bitmap data)
 * @example
 *     CCGraphic_Image img;
 *     CFBDGraphic_Point pos = {10, 20};
 *     CFBDGraphicSize sz = {32, 32};
 *     uint8_t bitmap[128] = {...};  // 32x32 pixels
 *     CFBDGraphic_InitImage(&img, &pos, &sz, bitmap);
 *     CFBDGraphic_DrawImage(device, &img);
 * @see CCGraphic_Image
 * @see CFBDGraphic_DrawImage
 */
void CFBDGraphic_InitImage(CCGraphic_Image* image,
                           CFBDGraphic_Point* tl_point,
                           CFBDGraphicSize* image_size,
                           uint8_t* sources_register);

/**
 * @brief Draw image widget on graphics device
 * @details Renders the image bitmap to the graphics device at the specified
 *          position. The image must be initialized with valid bitmap data.
 *          Drawing respects the device's current drawing mode (immediate vs deferred).
 * @param[in] handler - Graphics device to draw on
 * @param[in] image - Image widget to render
 * @return void
 * @note The image bitmap pointer must remain valid during draw operation
 * @note Device must be initialized and opened before calling this function
 * @example
 *     CFBD_GraphicDevice* dev = get_graphics_device();
 *     CCGraphic_Image logo;
 *     CFBDGraphic_InitImage(&logo, &pos, &size, logo_bitmap);
 *     CFBDGraphic_DrawImage(dev, &logo);
 *     dev->ops->update(dev);  // Commit if in deferred mode
 * @see CFBDGraphic_InitImage
 * @see CFBD_GraphicDevice
 */
void CFBDGraphic_DrawImage(CFBD_GraphicDevice* handler, CCGraphic_Image* image);

/**
 * @brief Draw image with clipping using small fixed buffer (batch processing)
 * @param device - Graphic device
 * @param image - Image to draw
 * @param clip_rect - Clipping rectangle (viewport bounds)
 * @note Uses 64-byte fixed buffer, processes image in columns batches
 */
void CFBDGraphic_DrawImageClipped(CFBD_GraphicDevice* device,
                                  CCGraphic_Image* image,
                                  CFBDGraphicRect* clip_rect);

/** @} */
