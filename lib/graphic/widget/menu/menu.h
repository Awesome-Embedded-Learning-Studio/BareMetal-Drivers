/**
 * @file menu.h
 * @brief Menu widget for hierarchical item selection
 * @details Provides comprehensive menu system with items, indicator, animation,
 *          and callbacks. Supports keyboard navigation and item selection.
 * @ingroup Graphics_Widget Graphics_Menu
 * @{
 */

#pragma once
#include <stddef.h>

#include "base/point.h"
#include "cfbd_define.h"
#include "cfbd_graphic_define.h"
#include "menu_indicator.h"
#include "menu_item.h"
#include "widget/animation/animation.h"
#include "widget/text_config.h"

/**
 * @brief Forward declaration of menu structure
 */
typedef struct _CFBD_Menu CFBD_Menu;

typedef struct
{
    /**
     * @brief
     * @note assigned_menu_items requests long lifetime exsits, recommends being static
     * @param pMenu
     * @param assigned_menu_items
     */
    void (*bind_item_groups_contains)(CFBD_Menu* pMenu, CFBD_MenuItemGroup* assigned_menu_items);

    CFBD_Bool (*add_item)(CFBD_Menu* pMenu,
                          const char* label,
                          Ascii_Font_Size size,
                          CFBD_MenuItemCallbackPack* callbacks);

    void (*set_indicator_property)(CFBD_Menu* pMenu, CFBD_MenuIndicator* pIndicator);
    /**
     * @brief
     * @note pBaseAnimation can be temp, internal assigns
     * @param pMenu
     * @param pBaseAnimation
     */
    void (*set_animation)(CFBD_Menu* pMenu, CFBD_BaseAnimation* pBaseAnimation);
    /**
     * @brief draw immediately
     *
     */
    void (*immediate_draw)(CFBD_Menu* pMenu);
    /**
     * @brief index selectors
     *
     */
    void (*select_index)(CFBD_Menu* pMenu, int index);
    /**
     * @brief User Activate the Selections, so things goes on
     *
     */
    void (*activate_current)(CFBD_Menu* pMenu);
    /**
     * @brief Reset the places
     * @note immediate draw requests,
     * including clear update and redraw is expected to behave!
     *
     */
    void (*reset_tl_points)(CFBD_Menu* pMenu, CFBDGraphic_Point* p, CFBD_Bool request_updates);
} CFBD_MenuOps;

/**
 * @typedef CFBD_Menu
 * @brief Main menu widget structure
 * @details Encompasses all menu state including items, rendering surface,
 *          indicator, animation, and selection state.
 * @see CFBD_InitMenu
 * @see CFBD_MenuOps
 */
typedef struct _CFBD_Menu
{
    /** @brief Menu operation table (function pointers) */
    CFBD_MenuOps* operations;

    /** @brief Graphics device for menu rendering */
    CFBD_GraphicDevice* device;

    /** @brief Menu items array and metadata */
    CFBD_MenuItemGroup* menu_items;

    /** @brief Top-left position of menu (default: 0,0) */
    CFBDGraphic_Point tl_point;

    /** @brief Maximum width constraint for menu items in pixels */
    SizeBaseType max_width;

    /** @brief Currently selected item index (0-based) */
    int selected;

    /** @brief Indicator bar configuration */
    CFBD_MenuIndicator indicator;

    /** @brief Animation timing configuration */
    CFBD_BaseAnimation animation;
} CFBD_Menu;

/**
 * @brief Initialize menu widget structure
 * @details Sets up menu with graphics device, item group, and constraints.
 *          Menu is ready to use after initialization.
 * @param[out] pMenu - Menu structure to initialize
 * @param[in] devices - Graphics device for rendering (must remain valid)
 * @param[in] assigned_menu_items - Item group array (must remain valid)
 * @param[in] max_width - Maximum width constraint in pixels
 * @return void
 * @note All pointer parameters must remain valid for menu lifetime
 * @example
 *     CFBD_Menu menu;
 *     CFBD_MenuItemGroup items = {...};  // Pre-allocated items
 *     CFBD_InitMenu(&menu, graphics_device, &items, 128);
 *     menu.operations->add_item(&menu, "Item 1", ASCII_8x16, NULL);
 *     menu.operations->immediate_draw(&menu);
 * @see CFBD_Menu
 * @see CFBD_MenuOps
 */
void CFBD_InitMenu(CFBD_Menu* pMenu,
                   CFBD_GraphicDevice* devices,
                   CFBD_MenuItemGroup* assigned_menu_items,
                   SizeBaseType max_width);

/**
 * @brief Select next menu item with wrapping
 * @details Moves selection down one item, wrapping to first item after last.
 * @param[in] m - Menu structure
 * @return void
 * @note Wraps around from last item to first
 * @example
 *     OLED_Menu_SelectNext(menu);  // Move to next item
 *     menu->operations->immediate_draw(menu);  // Redraw with new selection
 * @see OLED_Menu_SelectPrev
 */
static inline void OLED_Menu_SelectNext(CFBD_Menu* m)
{
    int ni = (m->selected + 1) % (int) m->menu_items->count;
    m->operations->select_index(m, ni);
}

/**
 * @brief Select previous menu item with wrapping
 * @details Moves selection up one item, wrapping to last item before first.
 * @param[in] m - Menu structure
 * @return void
 * @note Wraps around from first item to last
 * @example
 *     OLED_Menu_SelectPrev(menu);  // Move to previous item
 *     menu->operations->immediate_draw(menu);  // Redraw with new selection
 * @see OLED_Menu_SelectNext
 */
static inline void OLED_Menu_SelectPrev(CFBD_Menu* m)
{
    int ni = (m->selected - 1);
    if (ni < 0)
        ni = (int) m->menu_items->count - 1;
    m->operations->select_index(m, ni);
}

/** @} */
