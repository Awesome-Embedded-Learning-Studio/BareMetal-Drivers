#include "test_widget.h"

#include "base/point.h"
#include "base/size.h"
#include "config/system_settings.h"
#include "sys_clock/system_clock.h"
#include "widget/menu/menu.h"
#include "widget/text.h"
#include "widget/text_config.h"

static void test_text(CFBD_GraphicDevice* handler)
{
    CFBDGraphic_DeviceClearImmediate(handler);
    /* print sources */
    char* source = "Hello, World!This is Charlie's Speeking!";
    CFBDGraphic_Text item;
    CFBDGraphicSize screen_size;
    CFBDGraphic_GetScreenSize(handler, &screen_size);
    CFBDGraphic_Point p = {0, 0};
    CFBDGraphic_InitText(&item, p, screen_size, ASCII_6x8);

    CFBDGraphic_SetText(&item, source);
    CFBDGraphic_DrawText(handler, &item, CCGraphic_AsciiTextItem_AppendContinously);
    system_delay_ms(1000);

    /* after delay, print new_sources */
    char* new_source = "May I Help You?";
    CFBDGraphic_SetText(&item, new_source);
    CFBDGraphic_DrawText(handler, &item, CCGraphic_AsciiTextItem_AppendContinously);
    system_delay_ms(1000);

    // resets everythings
    CFBDGraphic_DeviceClearImmediate(handler);
    CFBDGraphic_SetTextIndexedPoint(&item, &p);

    new_source = "Yes!Please Offer me a lemon juice";
    CFBDGraphic_SetText(&item, new_source);
    CFBDGraphic_DrawText(handler, &item, CCGraphic_AsciiTextItem_AppendContinously);
}

void menu_example(CFBD_GraphicDevice* dev)
{
    CFBD_MenuItem items_buf[5];
    CFBD_Menu mymenu;
    CFBD_MenuItemGroup groups;
    groups.pItems = items_buf;
    groups.capacity = 5;
    groups.count = 0;

    CFBD_InitMenu(&mymenu, dev, &groups, 120);

    mymenu.operations->add_item(&mymenu, "Item 1", ASCII_6x8, NULL);
    mymenu.operations->add_item(&mymenu, "Item 2das", ASCII_6x8, NULL);
    mymenu.operations->add_item(&mymenu, "Item 3fasd", ASCII_6x8, NULL);
    mymenu.operations->add_item(&mymenu, "Item 4fffff", ASCII_6x8, NULL);
    mymenu.operations->add_item(&mymenu, "Item numbr", ASCII_6x8, NULL);

    mymenu.operations->immediate_draw(&mymenu);

    system_delay_ms(500);

    /* auto demo loop */
    while (1) {
        for (int i = 1; i < (int) mymenu.menu_items->count - 1; ++i) {
            mymenu.operations->select_index(&mymenu, i);
            system_delay_ms(600);
        }

        for (int i = (int) mymenu.menu_items->count - 3; i >= 0; --i) {
            mymenu.operations->select_index(&mymenu, i);
            system_delay_ms(600);
        }

        CFBDGraphic_Point p = {.x = 30, .y = 10};
        mymenu.operations->reset_tl_points(&mymenu, &p, CFBD_TRUE);

        for (int i = 1; i < (int) mymenu.menu_items->count - 1; ++i) {
            mymenu.operations->select_index(&mymenu, i);
            system_delay_ms(600);
        }

        for (int i = (int) mymenu.menu_items->count - 3; i >= 0; --i) {
            mymenu.operations->select_index(&mymenu, i);
            system_delay_ms(600);
        }

        p.x = 0;
        p.y = 0;
        mymenu.operations->reset_tl_points(&mymenu, &p, CFBD_TRUE);
    }
}

void test_widget(CFBD_GraphicDevice* handler)
{
    menu_example(handler);
}