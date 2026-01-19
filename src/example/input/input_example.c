#include <stddef.h>
#include <stdint.h>

#include "app.h"
#include "button_like/plain_button.h"
#include "cfbd_define.h"
#include "gpio.h"
#include "input.h"
#include "input_device_base.h"
#include "input_event.h"
#include "sys_clock/system_clock.h"

static CFBDInputEventContext input_context;
static CFBDInputDeviceBase fake_button;
CFBD_GPIOHandle led_handle;  // led handles here
CFBD_GPIOHandle led_handle2; // led handles here
CFBD_GPIOHandle key_handle;

void fake_make_events()
{
    CFBDInputEvent event;
    CFBD_CreateEvent(&event, &fake_button, 0);
    fake_button.ops->report_once(&fake_button, &event);
}

CFBD_Bool handle_click_registers(CFBDInputEvent* event)
{
    if (event->event_indicates == BUTTON_EVENT_CLICK) {
        CFBD_GPIOToggle(&led_handle);
    }
    return CFBD_TRUE;
}

CFBD_Bool handle_double_click_registers(CFBDInputEvent* event)
{
    if (event->event_indicates == BUTTON_EVENT_DOUBLE_CLICK) {
        CFBD_GPIOToggle(&led_handle2);
    }
    return CFBD_TRUE;
}

void asSyncPoll()
{
    // app inits
    getApp(CFBD_TRUE);
    CFBD_CreateGlobalContext(&input_context);

    CFBD_ButtonCreationPack pack = {.button_press_state = CFBD_FALSE,
                                    .callbacks = handle_click_registers,
                                    .input_context = &input_context,
                                    .isInterruptMode = CFBD_FALSE,
                                    .gpio = &key_handle};
    CFBD_PlainButton button;
    CFBD_CreatePlainButton(&button, &pack);
    input_context.ops->register_handle(&input_context, handle_double_click_registers);
    while (1) {
        CFBD_PlainButtonSyncScan(&button);
        input_context.ops->sync_poll(&input_context);
    }
}

void enable_intr()
{
    HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

CFBD_ButtonIntrCallbacks button_intr_callback;

void intr_check()
{
    // app inits
    getApp(CFBD_TRUE);
    CFBD_CreateGlobalContext(&input_context);

    CFBD_ButtonCreationPack pack = {.button_press_state = CFBD_FALSE,
                                    .callbacks = handle_click_registers,
                                    .input_context = &input_context,
                                    .isInterruptMode = CFBD_TRUE,
                                    .enable_intr = enable_intr,
                                    .gpio = &key_handle};

    CFBD_PlainButton button;
    CFBD_CreatePlainButton(&button, &pack);
    input_context.ops->register_handle(&input_context, handle_double_click_registers);

    // create monitorings
    CFBD_PlainButton* buttons[1] = {&button};
    IntrButtonArrayInfo info = {.array = buttons, .cnt = 1};
    CFBD_SetMonitoringButtons(&info);
    button_intr_callback = CFBD_PlainButtonGetIntrCallbacks();

    while (1) {
        // idle poll
        input_context.ops->sync_poll(&input_context);
    }
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (button_intr_callback != NULL) {
        button_intr_callback(&GPIO_Pin);
    }
}

int main()
{
    intr_check();
}