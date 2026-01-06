#pragma once
#include "base/point.h"
#include "widget/text.h"

typedef void (*CFBD_MenuItemCallback)(void* user_data);
typedef struct
{
    CFBD_MenuItemCallback callback;
    void* user_data;
} CFBD_MenuItemCallbackPack;

/* MenuItem */
typedef struct __CFBD_MenuItem
{
    char* label;                         /* pointer to zero-terminated string (owned by caller) */
    CFBDGraphic_Point tl;                /* top-left pixel of item label area */
    CFBD_MenuItemCallbackPack on_select; /* optional callback when this item activated */
    CFBDGraphic_Text text_obj;           /* internal text object (initialized on add) */
} CFBD_MenuItem;

typedef struct __CFBD_MenuItemArray
{
    CFBD_MenuItem* pItems;
    size_t capacity;
    size_t count;
} CFBD_MenuItemGroup;

void CFBD_MenuItemInit(CFBD_MenuItem* menuItem,
                       CFBDGraphic_Point* tl,
                       CFBD_MenuItemCallbackPack* callbacks);
