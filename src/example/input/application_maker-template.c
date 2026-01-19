#include "application_maker-template.h"

#include <stddef.h>
#include <stdint.h>

#include "app.h"
#include "cfbd_define.h"
#include "config/system_settings.h"
#include "gpio.h"
#include "sys_boot/boot.h"
#include "system/clock/clock_initer.h"

static MyBootArgs myBootArgs;
static CFBDBootTuple _tuple;

static uint32_t provide_clock_freq()
{
    return HAL_RCC_GetSysClockFreq();
}

static uint32_t provide_tick()
{
    return HAL_GetTick();
}

extern CFBD_GPIOHandle led_handle;
extern CFBD_GPIOHandle key_handle;
extern CFBD_GPIOHandle led_handle2;
void gpio_init()
{
    CFBD_ST_GPIOInitParams params = {.handle = GPIOA,
                                     .pin = GPIO_PIN_0,
                                     .initer = {.Pin = GPIO_PIN_0,
                                                .Mode = GPIO_MODE_OUTPUT_PP,
                                                .Speed = GPIO_SPEED_FREQ_LOW,
                                                .Pull = GPIO_PULLUP}};
    CFBD_GPIOInit(&led_handle, &params);

    CFBD_ST_GPIOInitParams led2params = {.handle = GPIOA,
                                         .pin = GPIO_PIN_1,
                                         .initer = {.Pin = GPIO_PIN_1,
                                                    .Mode = GPIO_MODE_OUTPUT_PP,
                                                    .Speed = GPIO_SPEED_FREQ_LOW,
                                                    .Pull = GPIO_PULLUP}};
    CFBD_GPIOInit(&led_handle2, &led2params);

    // CFBD_ST_GPIOInitParams key_params = {.handle = GPIOB,
    //                                      .pin = GPIO_PIN_0,
    //                                      .initer = {.Pin = GPIO_PIN_0,
    //                                                 .Mode = GPIO_MODE_INPUT,
    //                                                 .Speed = GPIO_SPEED_FREQ_LOW,
    //                                                 .Pull = GPIO_PULLUP}};

    CFBD_ST_GPIOInitParams key_params = {.handle = GPIOB,
                                         .pin = GPIO_PIN_0,
                                         .initer = {.Pin = GPIO_PIN_0,
                                                    .Mode = GPIO_MODE_IT_RISING_FALLING,
                                                    .Speed = GPIO_SPEED_FREQ_LOW,
                                                    .Pull = GPIO_PULLUP}};
    CFBD_GPIOInit(&key_handle, &key_params);
}

static CFBD_Bool appBooter(void* args)
{
    HAL_Init();
    system_clock_init();
    gpio_init();
    return CFBD_TRUE;
}

/**
 * @brief To make a specified boot, rewrite functions
 *
 * @return CFBD_BootStrapFunc
 */
CFBDBootTuple* CFBD_AppBootMaker()
{
    myBootArgs.should_led_init = 1;
    myBootArgs.shell_sleep = 500;

    _tuple.args = &myBootArgs;
    _tuple.boot_func = appBooter;

    return &_tuple;
}

CFBD_ClockFreqProvider CFBD_AppClockProvider(void)
{
    return provide_clock_freq;
}

CFBD_ClockTickProvider CFBD_AppTickProvider(void)
{
    return provide_tick;
}