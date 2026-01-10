/**
 * @file progressbar.h
 * @brief Progress bar widget for progress visualization
 * @details Provides progress bar structure and operations for displaying
 *          progress with optional borders, padding, and animations.
 * @ingroup Graphics_Widget Graphics_ProgressBar
 * @{
 */

#pragma once
#include <stdint.h>

#include "base/point.h"
#include "base/size.h"
#include "cfbd_define.h"
#include "cfbd_graphic_define.h"
#include "widget/animation/animation.h"

/**
 * @brief Forward declaration of progress bar structure
 */
typedef struct __CFBD_ProgressBar CFBD_ProgressBar;

/**
 * @typedef CFBD_ProgressBarOps
 * @brief Progress bar operation callbacks
 * @details Function pointers for progress bar operations including rendering
 *          and property configuration.
 * @see CFBD_ProgressBar
 */
typedef struct
{
    /**
     * @brief Render progress bar immediately
     * @details Draws current progress bar state with all visual properties.
     *          Updates display immediately if device in immediate mode.
     * @param pb - Progress bar structure
     * @return void
     * @example
     *     pb->ops->immediate_draw(pb);
     */
    void (*immediate_draw)(CFBD_ProgressBar*);

    /**
     * @brief Set progress bar value
     * @details Updates current progress value within [min, max] range.
     *          Automatically triggers visual update.
     * @param pb - Progress bar structure
     * @param new_value - New value (clamped to [min, max])
     * @return void
     * @example
     *     pb->ops->set_value(pb, 50);  // Set to 50% if range is [0, 100]
     * @see CFBD_ProgressBar
     */
    void (*set_value)(CFBD_ProgressBar*, int32_t new_value);

    /**
     * @brief Set progress bar property
     * @details Configures various visual properties of progress bar.
     *          Supported properties:
     *          - "border" (uint8_t*) - Border thickness (0 or 1)
     *          - "padding" (uint8_t*) - Inner padding (0-8 pixels)
     *          - "animation" (CFBD_BaseAnimation*) - Animation timing
     * @param pb - Progress bar structure
     * @param property - Property name (zero-terminated string)
     * @param args - Property value pointer (type depends on property)
     * @return @c CFBD_TRUE if property set successfully, @c CFBD_FALSE if unknown property
     * @example
     *     uint8_t border = 1;
     *     pb->ops->set_property(pb, "border", &border);
     *
     *     uint8_t padding = 2;
     *     pb->ops->set_property(pb, "padding", &padding);
     *
     *     CFBD_BaseAnimation anim = {8, 50};
     *     pb->ops->set_property(pb, "animation", &anim);
     * @see CFBD_BaseAnimation
     */
    CFBD_Bool (*set_property)(CFBD_ProgressBar*, const char* property, const void* args);
} CFBD_ProgressBarOps;

/**
 * @typedef CFBD_ProgressBar
 * @brief Progress bar widget structure
 * @details Encapsulates all progress bar state including value range,
 *          visual properties, animation, and rendering surface.
 * @see CFBD_ProgressBar_Init
 * @see CFBD_ProgressBarOps
 */
typedef struct __CFBD_ProgressBar
{
    /** @brief Progress bar operation table */
    CFBD_ProgressBarOps* ops;

    /** @brief Top-left position of progress bar in pixels */
    CFBDGraphic_Point tl;

    /** @brief Width and height dimensions of progress bar */
    CFBDGraphicSize size;

    /** @brief Minimum value of progress range */
    int32_t min;

    /** @brief Maximum value of progress range */
    int32_t max;

    /** @brief Current progress value (must be in [min, max]) */
    int32_t value;

    /** @brief Graphics device for rendering */
    CFBD_GraphicDevice* device;

    /** @brief Border display flag (0=no border, 1=display border) */
    uint8_t border;

    /** @brief Inner padding between border and fill in pixels */
    uint8_t padding;

    /** @brief Animation timing configuration for transitions */
    CFBD_BaseAnimation animation;
} CFBD_ProgressBar;

/**
 * @brief Initialize progress bar widget structure
 * @details Sets up progress bar with geometry, range, and graphics device.
 *          No dynamic allocation; all structures must be externally managed.
 * @param[out] pb - Progress bar structure to initialize
 * @param[in] device - Graphics device for rendering (must remain valid)
 * @param[in] tl - Top-left corner position in pixels
 * @param[in] sz - Width and height dimensions
 * @param[in] min - Minimum value of progress range
 * @param[in] max - Maximum value of progress range
 * @return void
 * @note Device, tl, and sz pointers must remain valid for pb lifetime
 * @note Initial value is set to min
 * @example
 *     CFBD_ProgressBar pb;
 *     CFBDGraphic_Point pos = {10, 50};
 *     CFBDGraphicSize sz = {108, 8};
 *     CFBD_ProgressBar_Init(&pb, device, &pos, &sz, 0, 100);
 *
 *     uint8_t border = 1, padding = 1;
 *     pb.ops->set_property(&pb, "border", &border);
 *     pb.ops->set_property(&pb, "padding", &padding);
 *
 *     // Update progress
 *     for (int i = 0; i <= 100; i += 10) {
 *         pb.ops->set_value(&pb, i);
 *         pb.ops->immediate_draw(&pb);
 *         HAL_Delay(100);
 *     }
 * @see CFBD_ProgressBar
 * @see CFBD_ProgressBarOps
 */
void CFBD_ProgressBar_Init(CFBD_ProgressBar* pb,
                           CFBD_GraphicDevice* device,
                           CFBDGraphic_Point* tl,
                           CFBDGraphicSize* sz,
                           int32_t min,
                           int32_t max);

/** @} */