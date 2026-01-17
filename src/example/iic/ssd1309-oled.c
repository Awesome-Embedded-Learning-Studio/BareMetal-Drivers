#include <stdlib.h>
#include <string.h>

#include "app.h"                        // CFBDApplication
#include "application_maker-template.h" // MyBootArgs
#include "benchmark/benchmark.h"
#include "cfbd_define.h"
#include "configs/external_impl_driver.h"
#include "device/graphic_device.h"
#include "driver/device/ssd1309/ssd1309.h"
#include "fast_test/test_base_graphic.h"
#include "fast_test/test_widget.h"
#include "iic_init.h"
#include "oled.h"
#include "sys_clock/system_clock.h" // system_delay_ms

static void test_set_pixel(CFBD_OLED* handle)
{
    for (uint8_t i = 0; i < 20; i++) {
        handle->ops->setPixel(handle, 2 * i, i);
        handle->ops->update(handle);
    }
    handle->ops->update(handle);
}

static void test_set_pixel_line(CFBD_OLED* handle, uint8_t xoffset, uint8_t y_offset)
{
    for (uint8_t i = 0; i < 20; i++)
        handle->ops->setPixel(handle, xoffset * i, y_offset * i);
    handle->ops->update(handle);
}

static void test_clear(CFBD_OLED* handle)
{
    handle->ops->clear(handle);
    handle->ops->update(handle);
}

static void test_framerate_visual(CFBD_OLED* handle)
{
    uint8_t x = 64, y = 32;       // 当前位置
    uint8_t old_x = x, old_y = y; // 上一帧位置
    int8_t dx = 2, dy = 1;        // 移动速度

    uint16_t width, height;
    handle->ops->self_consult(handle, "width", NULL, &width);
    handle->ops->self_consult(handle, "height", NULL, &height);

    const uint8_t dot_size = 5; // 光点大小

    while (1) {
        // 1. 清除旧位置（只清除旧光点区域）
        handle->ops->clear_area(handle, old_x, old_y, dot_size, dot_size);

        // 2. 更新位置
        x += dx;
        y += dy;

        // 边界反弹
        if (x >= width - dot_size || x <= 0)
            dx = -dx;
        if (y >= height - dot_size || y <= 0)
            dy = -dy;

        // 3. 画新位置
        for (uint8_t i = 0; i < dot_size; i++) {
            for (uint8_t j = 0; j < dot_size; j++) {
                handle->ops->setPixel(handle, x + i, y + j);
            }
        }

        // 4. 只刷新变化的区域（旧位置+新位置的包围盒）
        uint16_t update_x = (x < old_x) ? x : old_x;
        uint16_t update_y = (y < old_y) ? y : old_y;
        uint16_t update_w = abs(x - old_x) + dot_size;
        uint16_t update_h = abs(y - old_y) + dot_size;

        handle->ops->update_area(handle, update_x, update_y, update_w, update_h);

        // 记录当前位置为下一帧的旧位置
        old_x = x;
        old_y = y;
    }
}

CFBD_OLED ssd1309;

void do1309Demo()
{
    CFBDApplication* app = getApp(CFBD_TRUE);
    (void) app;

    CFBD_OLED_IICInitsParams params;
    CFBD_I2CHandle handle;
    CFBD_ST_I2CPrivate* priv = getIICPrivateInits();
    stm32_i2c_bus_register(&handle, priv);

    params.device_specifics = getSSD1309Specific();
    params.accepted_time_delay = -1; // never time outs
    params.device_address = SSD1309_DRIVER_ADDRESS;
    params.i2cHandle = &handle;
    params.iic_transition_callback = NULL;

    CFBD_GetOLEDHandle(&ssd1309, CFBD_OLEDDriverType_IIC, &params, CFBD_TRUE);

    CFBD_GraphicDevice graphic_device;
    CFBDGraphic_BindDevice(&graphic_device, OLED, &ssd1309);
    CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(&graphic_device, CFBD_TRUE);

    // test_framerate_visual(&ssd1309);
    // test_draw_base_graphic(&graphic_device);

    // system_delay_ms(1000);

    // test_widget(&graphic_device);

    // system_delay_ms(1000);
    CFBDGraphic_DeviceClearImmediate(&graphic_device);
    test_fps(&graphic_device);
    // test_widget(&graphic_device);
    while (1)
        ;
}