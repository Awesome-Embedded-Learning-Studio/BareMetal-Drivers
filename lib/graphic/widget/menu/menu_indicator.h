/**
 * @file menu_indicator.h
 * @brief Menu indicator widget for visual selection feedback
 * @details Provides menu indicator structure and functions for rendering
 *          a sliding indicator bar showing current menu selection.
 * @ingroup Graphics_Widget Graphics_Menu Graphics_MenuIndicator
 * @{
 */

#pragma once
#include <stdint.h>

#include "cfbd_define.h"
#include "cfbd_graphic_define.h"

/**
 * @brief Forward declaration of menu structure
 */
typedef struct _CFBD_Menu CFBD_Menu;

/**
 * @typedef CFBD_MenuIndicator
 * @brief Menu indicator configuration structure
 * @details Defines visual properties of the menu indicator bar that
 *          slides between menu items to show current selection.
 * @see CFBD_InitDefaultMenuIndicator
 * @see CFBD_DrawMenuIndicator
 */
typedef struct __CFBD_MenuIndicator
{
    /** @brief Width of sliding indicator bar in pixels
     *  @details Set to 0 to disable indicator drawing */
    uint8_t width;

    /** @brief X-position (left edge) of indicator in pixels */
    uint8_t x;
} CFBD_MenuIndicator;

/**
 * @brief Initialize menu indicator with default values
 * @details Sets up a menu indicator with default configuration from
 *          menu_config.h macros (CFBD_MENU_INDICATOR_WIDTH, CFBD_MENU_INDICATOR_POSX).
 * @param[out] menuIndicatorDefault - Indicator structure to initialize
 * @return void
 * @note Should be called before using indicator with menu
 * @example
 *     CFBD_MenuIndicator indicator;
 *     CFBD_InitDefaultMenuIndicator(&indicator);
 *     // indicator.width = CFBD_MENU_INDICATOR_WIDTH (4 by default)
 *     // indicator.x = CFBD_MENU_INDICATOR_POSX (0 by default)
 * @see CFBD_MenuIndicator
 * @see CFBD_DrawMenuIndicator
 */
void CFBD_InitDefaultMenuIndicator(CFBD_MenuIndicator* menuIndicatorDefault);

/**
 * @brief Draw menu indicator on menu
 * @details Renders the sliding indicator bar at specified area on the menu.
 *          Called internally by menu rendering operations.
 * @param[in] menu - Menu containing the indicator
 * @param[in] x - Left edge X coordinate of area
 * @param[in] y - Top edge Y coordinate of area
 * @param[in] w - Width of area containing indicator
 * @param[in] h - Height of area containing indicator
 * @return @c CFBD_TRUE on successful draw, @c CFBD_FALSE on error
 * @note Respects menu indicator configuration (width, x position)
 * @note If indicator.width is 0, no indicator is drawn
 * @example
 *     CFBD_MenuIndicator ind;
 *     CFBD_InitDefaultMenuIndicator(&ind);
 *     if (CFBD_DrawMenuIndicator(menu, 0, 0, 128, 64)) {
 *         menu->device->ops->update(menu->device);
 *     }
 * @see CFBD_MenuIndicator
 * @see CFBD_InitDefaultMenuIndicator
 */
CFBD_Bool CFBD_DrawMenuIndicator(CFBD_Menu* menu, uint16_t x, uint16_t y, uint16_t w, uint16_t h);

/** @} */