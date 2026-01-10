/**
 * @file menu_config.h
 * @brief Menu widget compile-time configuration
 * @details Provides configuration macros for menu appearance, including
 *          indicator dimensions, positioning, and item spacing.
 * @ingroup Graphics_Widget Graphics_Menu
 * @{
 */

#pragma once

/**
 * @defgroup Graphics_MenuConfig Menu Configuration
 * @ingroup Graphics_Menu
 * @brief Configuration parameters for menu widget appearance
 * @{
 */

/**
 * @brief Default menu indicator width in pixels
 * @details Default sliding indicator bar width. Set to 0 to disable indicator.
 * @note Valid range: 0-8 pixels
 * @see CFBD_MENU_INDICATOR_MAX_WIDTH_PX
 */
#define CFBD_MENU_INDICATOR_WIDTH (4)

/**
 * @brief Default menu indicator X position
 * @details Left edge position of indicator bar in pixels.
 * @note Typically 0 (left edge of menu)
 * @see CFBD_MenuIndicator
 */
#define CFBD_MENU_INDICATOR_POSX (0)

/**
 * @brief Maximum menu indicator width in pixels
 * @details Hard limit for indicator width, prevents indicator from
 *          exceeding this value regardless of configuration.
 */
#define CFBD_MENU_INDICATOR_MAX_WIDTH_PX 8

/**
 * @brief Maximum menu indicator height in pixels
 * @details Hard limit for indicator height to match typical menu item height.
 */
#define CFBD_MENU_INDICATOR_MAX_HEIGHT_PX 16

/**
 * @brief Horizontal gap between indicator and menu items in pixels
 * @details Spacing between sliding indicator bar and item text.
 */
#define CFBD_MENU_INDICATOR_GAP (4)

/**
 * @brief Vertical gap between menu items in pixels
 * @details Line spacing in pixels between consecutive menu items.
 * @note Set to 0 for tight packing, positive values for visual separation.
 */
#define CFBD_MENU_ITEM_Y_GAP (0)

/** @} */