#include "test_widget.h"

#include <stdint.h>

#include "app.h"
#include "base/point.h"
#include "base/size.h"
#include "cfbd_define.h"
#include "config/system_settings.h"
#include "sys_clock/system_clock.h"
#include "widget/animation/animation.h"
#include "widget/menu/menu.h"
#include "widget/progressbar/progressbar.h"
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

/*
 * Demo: ProgressBar usage
 * OLED: 128x64
 * Effect:
 *  - Title text
 *  - Main progress bar (0 -> 100, smooth + flow light)
 *  - Sub progress bar (busy animation in stages)
 */

void progressbar_usage(CFBD_GraphicDevice* dev)
{
    /* =======================
     * Title
     * ======================= */
    CFBDGraphic_Text title;
    CFBDGraphic_Point tp = {.x = 12, .y = 2};
    CFBDGraphicSize ts = {.width = 104, .height = 10};
    CFBDGraphic_InitText(&title, tp, ts, ASCII_6x8);
    CFBDGraphic_SetText(&title, "System Booting...");

    /* =======================
     * Main ProgressBar
     * ======================= */
    CFBD_ProgressBar main_pb;
    CFBDGraphic_Point p = {6, 18};
    CFBDGraphicSize sz = {116, 12};
    CFBD_ProgressBar_Init(&main_pb, dev, &p, &sz, 0, 100);
    uint8_t boarder_and_padding = 1;
    CFBD_BaseAnimation animation = {.anim_frame_delay_ms = 25, .anim_frames = 10};
    main_pb.ops->set_property(&main_pb, "border", &boarder_and_padding);
    main_pb.ops->set_property(&main_pb, "padding", &boarder_and_padding);
    main_pb.ops->set_property(&main_pb, "animation", &animation);

    /* =======================
     * Stage ProgressBar
     * ======================= */
    CFBD_ProgressBar stage_pb;
    CFBDGraphic_Point p2 = {20, 38};
    CFBDGraphicSize sz2 = {88, 8};
    CFBD_ProgressBar_Init(&stage_pb, dev, &p2, &sz2, 0, 100);
    stage_pb.ops->set_property(&stage_pb, "border", &boarder_and_padding);
    stage_pb.ops->set_property(&stage_pb, "padding", &boarder_and_padding);
    stage_pb.ops->set_property(&stage_pb, "animation", &animation);
    /* =======================
     * First frame draw
     * ======================= */
    dev->ops->clear(dev);

    CFBDGraphic_DrawText(dev, &title, CCGraphic_AsciiTextItem_RequestOldPoint);

    main_pb.ops->immediate_draw(&main_pb);
    main_pb.ops->immediate_draw(&stage_pb);

    dev->ops->update(dev);

    /* =======================
     * Demo animation
     * ======================= */
    const int stages[] = {0, 20, 45, 70, 100};

    while (1) {
        for (int s = 0; s < 5; ++s) {
            /* main progress moves forward */
            main_pb.ops->set_value(&main_pb, stages[s]);
            system_delay_ms(500);

            /* stage progress busy animation */
            stage_pb.ops->set_value(&stage_pb, 25);
            system_delay_ms(500);

            stage_pb.ops->set_value(&stage_pb, 50);
            system_delay_ms(500);

            stage_pb.ops->set_value(&stage_pb, 75);
            system_delay_ms(500);

            stage_pb.ops->set_value(&stage_pb, 100);
            system_delay_ms(500);
            stage_pb.ops->set_value(&stage_pb, 0);
        }
    }
}

void test_widget(CFBD_GraphicDevice* handler)
{
    progressbar_usage(handler);
}