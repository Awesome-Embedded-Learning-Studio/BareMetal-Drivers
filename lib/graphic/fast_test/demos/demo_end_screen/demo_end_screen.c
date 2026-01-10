/**
 * @file demo_end_screen.c
 * @brief Final demo end screen
 *
 * @details
 * Displays a simple "DEMO END" message.
 * Intended to be the last demo in queue mode.
 */

#include "device/graphic_device.h"
#include "sys_clock/system_clock.h"
#include "widget/text.h"

void demo_end_screen(CFBD_GraphicDevice* dev)
{
    dev->ops->clear(dev);

    CFBDGraphic_Text title;
    CFBDGraphic_Point tp = {.x = 12, .y = 2};
    CFBDGraphicSize ts = {.width = 104, .height = 10};
    CFBDGraphic_InitText(&title, tp, ts, ASCII_6x8);
    CFBDGraphic_SetText(&title, "End Of the Everythings");

    CFBDGraphic_DrawText(dev, &title, CCGraphic_AsciiTextItem_RequestOldPoint);

    system_delay_ms(5000);
}
