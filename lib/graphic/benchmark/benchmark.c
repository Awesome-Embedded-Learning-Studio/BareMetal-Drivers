
#include <stdio.h>
#include <string.h>

#include "base/point.h"
#include "base/size.h"
#include "config/system_settings.h"
#include "device/graphic_device.h"
#include "sys_clock/system_clock.h"
#include "widget/text.h"

// FPS性能测试 - 在OLED屏幕上显示刷新率
static void test_fps_benchmark(CFBD_GraphicDevice* handler)
{
    CFBDGraphicSize screen_size;
    CFBDGraphic_GetScreenSize(handler, &screen_size);

    CFBDGraphic_Text fps_text;
    CFBDGraphic_Point p = {0, 0};
    CFBDGraphic_InitText(&fps_text, p, screen_size, ASCII_6x8);

    uint32_t frame_count = 0;
    uint32_t last_time = HAL_GetTick();

    uint32_t fps_x10 = 0; // FPS * 10（定点数）
    char buffer[64];

    uint32_t test_start = HAL_GetTick();
    uint32_t test_duration = 30000;

    while ((HAL_GetTick() - test_start) < test_duration) {
        // CFBDGraphic_DeviceClearImmediate(handler);

        frame_count++;
        uint32_t current_time = HAL_GetTick();
        uint32_t elapsed = current_time - last_time;

        // 每 500ms 更新一次 FPS
        if (elapsed >= 500) {
            // fps_x10 = frame_count * 1000 * 10 / elapsed
            fps_x10 = (frame_count * 10000U) / elapsed;

            frame_count = 0;
            last_time = current_time;
        }

        snprintf(buffer,
                 sizeof(buffer),
                 "FPS: %lu.%lu\n"
                 "Time: %lus\n",
                 fps_x10 / 10,
                 fps_x10 % 10,
                 (HAL_GetTick() - test_start) / 1000);

        CFBDGraphic_SetText(&fps_text, buffer);
        CFBDGraphic_DrawText(handler, &fps_text, CCGraphic_AsciiTextItem_RequestOldPoint);
    }

    CFBDGraphic_DeviceClearImmediate(handler);
    snprintf(buffer,
             sizeof(buffer),
             "Test Complete!\n"
             "Final FPS: %lu.%lu",
             fps_x10 / 10,
             fps_x10 % 10);

    CFBDGraphic_SetText(&fps_text, buffer);
    CFBDGraphic_DrawText(handler, &fps_text, CCGraphic_AsciiTextItem_RequestOldPoint);
}

void test_fps(CFBD_GraphicDevice* handler)
{
    test_fps_benchmark(handler);
}