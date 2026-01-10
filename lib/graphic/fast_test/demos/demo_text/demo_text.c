#include "device/graphic_device.h"
#include "sys_clock/system_clock.h"
#include "widget/text.h"

void test_text(CFBD_GraphicDevice* handler)
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
