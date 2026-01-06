#pragma once
#include <stddef.h>

#include "base/point.h"
#include "cfbd_define.h"
#include "cfbd_graphic_define.h"
#include "menu_indicator.h"
#include "menu_item.h"
#include "widget/animation/animation.h"
#include "widget/text_config.h"
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

/* Menu */
typedef struct _CFBD_Menu
{
    CFBD_MenuOps* operations;
    CFBD_GraphicDevice* device;
    CFBD_MenuItemGroup* menu_items;
    CFBDGraphic_Point tl_point; /* default from 0, 0 */
    SizeBaseType max_width;     // max widths
    int selected;               /* current selected index */

    /* indicator config */
    CFBD_MenuIndicator indicator;
    /* animation config */
    CFBD_BaseAnimation animation;
} CFBD_Menu;

void CFBD_InitMenu(CFBD_Menu* pMenu,
                   CFBD_GraphicDevice* devices,
                   CFBD_MenuItemGroup* assigned_menu_items,
                   SizeBaseType max_width);

static inline void OLED_Menu_SelectNext(CFBD_Menu* m)
{
    int ni = (m->selected + 1) % (int) m->menu_items->count;
    m->operations->select_index(m, ni);
}

static inline void OLED_Menu_SelectPrev(CFBD_Menu* m)
{
    int ni = (m->selected - 1);
    if (ni < 0)
        ni = (int) m->menu_items->count - 1;
    m->operations->select_index(m, ni);
}
