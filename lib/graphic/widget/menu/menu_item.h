/**
 * @file menu_item.h
 * @brief Menu item widget for menu selection options
 * @details Provides menu item structures and initialization for creating
 *          clickable menu options with optional callbacks.
 * @ingroup Graphics_Widget Graphics_Menu Graphics_MenuItem
 * @{
 */

#pragma once
#include "base/point.h"
#include "widget/text.h"

/**
 * @typedef CFBD_MenuItemCallback
 * @brief Callback function type for menu item activation
 * @details Called when user selects/activates a menu item.
 * @param[in] user_data - User-supplied callback context data
 * @return void
 * @example
 *     void on_start_selected(void* user_data) {
 *         int* app_state = (int*)user_data;
 *         *app_state = STATE_RUNNING;
 *     }
 * @see CFBD_MenuItemCallbackPack
 */
typedef void (*CFBD_MenuItemCallback)(void* user_data);

/**
 * @typedef CFBD_MenuItemCallbackPack
 * @brief Callback and user data wrapper for menu items
 * @details Encapsulates callback function and user data context pointer.
 * @see CFBD_MenuItemCallback
 */
typedef struct
{
    /** @brief Callback function to invoke on item selection */
    CFBD_MenuItemCallback callback;

    /** @brief User data passed to callback (optional) */
    void* user_data;
} CFBD_MenuItemCallbackPack;

/**
 * @typedef CFBD_MenuItem
 * @brief Single menu item structure
 * @details Represents one menu item with label, position, and optional callback.
 * @see CFBD_MenuItemInit
 * @see CFBD_MenuItemCallbackPack
 */
typedef struct __CFBD_MenuItem
{
    /** @brief Menu item display label (zero-terminated string, owned by caller) */
    char* label;

    /** @brief Top-left pixel position of item label area */
    CFBDGraphic_Point tl;

    /** @brief Optional callback invoked when item is activated/selected */
    CFBD_MenuItemCallbackPack on_select;

    /** @brief Internal text rendering object (initialized on menu add) */
    CFBDGraphic_Text text_obj;
} CFBD_MenuItem;

/**
 * @typedef CFBD_MenuItemGroup
 * @brief Array container for menu items
 * @details Manages dynamic array of menu items with capacity and count tracking.
 * @see CFBD_MenuItem
 */
typedef struct __CFBD_MenuItemArray
{
    /** @brief Array of menu items */
    CFBD_MenuItem* pItems;

    /** @brief Total allocated capacity */
    size_t capacity;

    /** @brief Current number of items in group */
    size_t count;
} CFBD_MenuItemGroup;

/**
 * @brief Initialize a menu item structure
 * @details Sets up a menu item with position and optional selection callback.
 *          The label should be set separately before adding to menu.
 * @param[out] menuItem - Menu item structure to initialize
 * @param[in] tl - Top-left position in pixels (can be NULL, set later)
 * @param[in] callbacks - Callback pack with function and user data (can be NULL)
 * @return void
 * @note Label must be set by caller before rendering
 * @note Callback pack can be NULL if no callback desired
 * @example
 *     CFBD_MenuItem item = {0};
 *     CFBDGraphic_Point pos = {0, 0};
 *     CFBD_MenuItemCallbackPack cb = {on_item_select, &app_state};
 *     CFBD_MenuItemInit(&item, &pos, &cb);
 *     item.label = "Start Game";
 * @see CFBD_MenuItem
 * @see CFBD_MenuItemCallbackPack
 */
void CFBD_MenuItemInit(CFBD_MenuItem* menuItem,
                       CFBDGraphic_Point* tl,
                       CFBD_MenuItemCallbackPack* callbacks);

/** @} */
