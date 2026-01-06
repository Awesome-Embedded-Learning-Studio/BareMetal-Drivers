#include "app.h" // CFBDApplication
#include "application_maker-template.h" // MyBootArgs
#include "sys_clock/system_clock.h" // system_delay_ms
#include "gpio.h" // GPIO, CFBD_GPIOToggle

int main()
{
    CFBDApplication* app = getApp(CFBD_TRUE);
    // app is boot
    MyBootArgs* myArgs = (MyBootArgs*)app->bootargs;
    const uint32_t sleepy_time = myArgs->shell_sleep;

    CFBD_ST_GPIOInitParams params = {
        .handle = GPIOC,
        .pin = GPIO_PIN_13,
        .initer = {
            .Pin = GPIO_PIN_13,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Pull = GPIO_PULLUP
        }
    };
    CFBD_GPIOHandle led_handle;
    CFBD_GPIOInit(&led_handle, &params);

    while(1){
        CFBD_GPIOToggle(&led_handle);
        system_delay_ms(sleepy_time);
    }
}