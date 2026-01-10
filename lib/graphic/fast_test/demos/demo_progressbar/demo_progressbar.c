/**
 * @file demo_progressbar.c
 * @author CharlieChen
 * @brief Progress bar widget demonstration
 *
 * @details
 * Demonstrates:
 *  - Linear progress bar rendering
 *  - Smooth incremental animation
 *  - No visual artifacts (no flowing light)
 *
 * This demo automatically completes and returns.
 */

#include "sys_clock/system_clock.h"
#include "widget/progressbar/progressbar.h"
#include "widget/text.h"

void demo_progressbar(CFBD_GraphicDevice* dev)
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

    for (int s = 0; s < 5; ++s) {
        /* main progress moves forward */
        main_pb.ops->set_value(&main_pb, stages[s]);
        system_delay_ms(100);

        /* stage progress busy animation */
        stage_pb.ops->set_value(&stage_pb, 25);
        system_delay_ms(100);

        stage_pb.ops->set_value(&stage_pb, 50);
        system_delay_ms(100);

        stage_pb.ops->set_value(&stage_pb, 75);
        system_delay_ms(500);

        stage_pb.ops->set_value(&stage_pb, 100);
        system_delay_ms(100);
        stage_pb.ops->set_value(&stage_pb, 0);
    }
}
