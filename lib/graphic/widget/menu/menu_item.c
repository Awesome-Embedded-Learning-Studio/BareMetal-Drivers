#include "menu_item.h"

void CFBD_MenuItemInit(CFBD_MenuItem* menuItem,
                       CFBDGraphic_Point* tl,
                       CFBD_MenuItemCallbackPack* callbacks)
{
    menuItem->tl = *tl;
    menuItem->on_select = *callbacks;
}