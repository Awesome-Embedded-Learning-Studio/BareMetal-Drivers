/**
 * @file icontext_menu.h
 * @brief Icon-text menu widget for scrollable menu display
 * @details Provides a horizontal scrollable menu widget optimized for small displays,
 *          featuring icon+text menu items with smooth scrolling animation
 *          and gesture-based navigation.
 * @ingroup Graphics_Widget Graphics_Menu
 * @{
 */

#pragma once
#include <stddef.h>
#include <stdint.h>

#include "base/point.h"
#include "base/rectangle.h"
#include "base/size.h"
#include "cfbd_define.h"
#include "cfbd_graphic_define.h"
#include "menu_item.h"
#include "widget/animation/animation.h"
#include "widget/base_support/image.h"
#include "widget/text.h"

/**
 * @brief Forward declaration of icon-text menu structure
 */
typedef struct _CFBD_IconTextMenu CFBD_IconTextMenu;

/**
 * @defgroup IconTextMenu Icon-Text Menu Widget
 * @ingroup Graphics_Widget Graphics_Menu
 * @brief Horizontal scrollable menu for icon+text display
 * @details Menu system with icon+text items, designed for small screens
 *          with smooth animations and gesture-based navigation.
 * @{
 */

/**
 * @brief Configuration constants for icon-text menu
 */
#define CFBD_ICONTEXT_MENU_DEFAULT_ITEM_WIDTH (60)     ///< Width of each menu item
#define CFBD_ICONTEXT_MENU_DEFAULT_ITEM_HEIGHT (80)    ///< Height of each menu item
#define CFBD_ICONTEXT_MENU_ICON_SIZE (48)              ///< Icon bitmap dimensions
#define CFBD_ICONTEXT_MENU_ICON_TEXT_GAP (4)           ///< Gap between icon and text
#define CFBD_ICONTEXT_MENU_TEXT_SIZE ASCII_6x8         ///< Font size for labels
#define CFBD_ICONTEXT_MENU_ANIMATION_DURATION_MS (300) ///< Scroll animation duration
#define CFBD_ICONTEXT_MENU_VISIBLE_ITEMS (3)           ///< Number of visible items

/**
 * @typedef CFBD_IconTextMenuItemState
 * @brief Menu item visual state enumeration
 * @details Indicates the current visual state of a menu item
 */
typedef enum
{
    CFBD_ICONTEXT_ITEM_NORMAL = 0,  ///< Default unselected state
    CFBD_ICONTEXT_ITEM_FOCUSED = 1, ///< Item has focus/is selected
    CFBD_ICONTEXT_ITEM_PRESSED = 2, ///< Item is being pressed/activated
    CFBD_ICONTEXT_ITEM_DISABLED = 3 ///< Item is disabled/unavailable
} CFBD_IconTextMenuItemState;

/**
 * @typedef CFBD_IconTextScrollDirection
 * @brief Scroll direction enumeration
 */
typedef enum
{
    CFBD_ICONTEXT_SCROLL_LEFT = -1, ///< Scroll towards previous items
    CFBD_ICONTEXT_SCROLL_RIGHT = 1, ///< Scroll towards next items
    CFBD_ICONTEXT_SCROLL_NONE = 0   ///< No scrolling
} CFBD_IconTextScrollDirection;

/**
 * @typedef CFBD_IconTextMenuItem
 * @brief Single menu item structure with icon and label
 * @details Represents one scrollable menu item containing icon bitmap and text label
 */
typedef struct
{
    /** @brief Item label text (owned by caller, must be static) */
    const char* label;

    /** @brief Icon bitmap data (48x48 pixels, owned by caller) */
    uint8_t* icon_bitmap;

    /** @brief Icon dimensions (typically 48x48) */
    CFBDGraphicSize icon_size;

    /** @brief Current visual state of item */
    CFBD_IconTextMenuItemState state;

    /** @brief Optional callback when item is selected */
    CFBD_MenuItemCallbackPack callback;

    /** @brief Calculated top-left position for rendering */
    CFBDGraphic_Point render_position;

    /** @brief Image widget for icon rendering (internal) */
    CCGraphic_Image icon_widget;

    /** @brief Text widget for label rendering (internal) */
    CFBDGraphic_Text text_widget;

    /** @brief User-defined data pointer */
    void* user_data;
} CFBD_IconTextMenuItem;

/**
 * @brief Operation table for icon-text menu
 * @details Virtual function table for icon-text menu operations
 */
typedef struct _CFBD_IconTextMenuOps
{
    /**
     * @brief Add menu item
     */
    CFBD_Bool (*add_item)(CFBD_IconTextMenu* pMenu,
                          const char* label,
                          uint8_t* icon_bitmap,
                          CFBDGraphicSize* icon_size,
                          CFBD_MenuItemCallbackPack* callback);

    /**
     * @brief Scroll menu in specified direction
     */
    void (*scroll)(CFBD_IconTextMenu* pMenu, CFBD_IconTextScrollDirection direction);

    /**
     * @brief Select item by index
     */
    CFBD_Bool (*select_index)(CFBD_IconTextMenu* pMenu, int index);

    /**
     * @brief Activate current item
     */
    void (*activate_current)(CFBD_IconTextMenu* pMenu);

    /**
     * @brief Draw menu
     */
    void (*immediate_draw)(CFBD_IconTextMenu* pMenu);

    /**
     * @brief Update animation state
     */
    CFBD_Bool (*update)(CFBD_IconTextMenu* pMenu, uint32_t delta_ms);

    /**
     * @brief Set appearance properties
     */
    void (*set_appearance)(CFBD_IconTextMenu* pMenu,
                           uint32_t bg_color,
                           uint32_t highlight_color,
                           uint32_t text_color);

    /**
     * @brief Set item dimensions
     */
    void (*set_dimensions)(CFBD_IconTextMenu* pMenu,
                           SizeBaseType width,
                           SizeBaseType height,
                           uint16_t spacing);

    /**
     * @brief Get selected item
     */
    CFBD_IconTextMenuItem* (*get_selected_item)(CFBD_IconTextMenu* pMenu);

    /**
     * @brief Get item by index
     */
    CFBD_IconTextMenuItem* (*get_item)(CFBD_IconTextMenu* pMenu, int index);

    /**
     * @brief Set circular scrolling
     */
    void (*set_circular)(CFBD_IconTextMenu* pMenu, CFBD_Bool is_circular);

    /**
     * @brief Animated scroll
     */
    void (*animate_scroll)(CFBD_IconTextMenu* pMenu, CFBD_IconTextScrollDirection direction);

    void (*set_selection_border)(CFBD_IconTextMenu* pMenu, uint16_t width);
} CFBD_IconTextMenuOps;

/**
 * @typedef CFBD_IconTextMenu
 * @brief Main icon-text menu widget structure
 * @details Manages scrollable menu state including items, animations,
 *          selection, and scroll position.
 */
typedef struct _CFBD_IconTextMenu
{
    /** @brief Menu operation table (function pointers) */
    CFBD_IconTextMenuOps* operations;

    /** @brief Array of menu items */
    CFBD_IconTextMenuItem* items;

    /** @brief Total number of items in menu */
    size_t item_count;

    /** @brief Maximum capacity of items array */
    size_t capacity;

    /** @brief Graphics device for rendering */
    CFBD_GraphicDevice* device;

    /** @brief Menu viewport rectangle (where menu is displayed) */
    CFBDGraphicRect viewport;

    /** @brief Currently selected item index */
    int selected_index;

    /** @brief First visible item index */
    int first_visible_index;

    /** @brief Current scroll offset in pixels (smooth scrolling) */
    int16_t scroll_offset;

    /** @brief Target scroll offset (used by animations) */
    int16_t target_scroll_offset;

    /** @brief Current scroll direction */
    CFBD_IconTextScrollDirection scroll_direction;

    /** @brief Animation state for smooth scrolling */
    CFBD_BaseAnimation scroll_animation;

    /** @brief Width of each menu item in pixels */
    SizeBaseType item_width;

    /** @brief Height of each menu item in pixels */
    SizeBaseType item_height;

    /** @brief Spacing between menu items in pixels */
    uint16_t item_spacing;

    /** @brief Whether menu wraps around at edges */
    CFBD_Bool is_circular;

    /** @brief Animation currently in progress */
    CFBD_Bool is_animating;

    /** @brief Animation start offset */
    int16_t animation_start_offset;

    /** @brief Animation elapsed time in ms */
    uint32_t animation_elapsed_ms;

    /** @brief Previous frame scroll offset (for dirty region detection) */
    int16_t prev_scroll_offset;

    /** @brief Current animation frame counter (0 to anim_frames-1) */
    uint8_t anim_current_frame;

    uint16_t selection_border_width;
} CFBD_IconTextMenu;

/**
 * @brief Initialize icon-text menu widget
 * @details Configures a new icon-text menu with caller-provided items buffer.
 *          No dynamic allocation - caller manages buffer memory.
 * @param[out] pMenu - Menu structure to initialize
 * @param[in] device - Graphics device to render on
 * @param[in] viewport - Display area for menu (rectangle)
 * @param[in] items_buffer - Caller-provided buffer for menu items
 * @param[in] capacity - Maximum number of items that buffer can hold
 * @return void
 *
 * @note Items buffer must remain valid for lifetime of menu
 * @note Recommended to use stack-allocated array for items buffer
 *
 * @example
 *     CFBD_IconTextMenuItem items[6];  // Stack allocation
 *     CFBD_IconTextMenu menu;
 *     CFBDGraphicRect view = {{0, 24}, {128, 104}};
 *     CFBD_InitIconTextMenu(&menu, device, &view, items, 6);
 *     menu.operations->add_item(&menu, \"Home\", icon_data, &icon_size, NULL);
 *
 * @see CFBD_IconTextMenu
 * @see CFBD_IconTextMenuOps
 */
void CFBD_InitIconTextMenu(CFBD_IconTextMenu* pMenu,
                           CFBD_GraphicDevice* device,
                           CFBDGraphicRect* viewport,
                           CFBD_IconTextMenuItem* items_buffer,
                           size_t capacity);

/** @} */

/** @} */
