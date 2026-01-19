#include "plain_button.h"

#include "app.h"
#include "cfbd_define.h"
#include "input_device_base.h"
#include "plain_button_impl.h"

void CFBD_CreatePlainButton(CFBD_PlainButton* button, CFBD_ButtonCreationPack* pack)
{
    CFBDInputEventContext* context = pack->input_context;
    button->handle = pack->gpio;
    button->long_press_triggered = CFBD_FALSE;
    button->press_time = 0;
    button->release_time = 0;
    button->last_click_time = 0;
    button->is_pressed = CFBD_FALSE;
    button->current_state_id = BUTTON_EVENT_IDLE;
    button->is_double_click_pending = CFBD_FALSE;
    button->click_pending_time = 0;
    CFBD_CreateInputDeviceBase(&button->base_inputs, context, button->handle);
    context->ops->register_handle(context, pack->callbacks);

    if (pack->isInterruptMode) {
        button->mode = BUTTON_MODE_INTERRUPT;
        pack->enable_intr(button);
    }
    else {
        button->mode = BUTTON_MODE_POLLING;
    }
}

static void ButtonOfLongSession(CFBD_PlainButton* handle)
{
    uint32_t current_time = HAL_GetTick();

    // 长按检测
    if (handle->is_pressed && !handle->long_press_triggered) {
        uint32_t press_duration = current_time - handle->press_time;

        if (press_duration >= BUTTON_LONG_PRESS_TIME) {
            handle->long_press_triggered = CFBD_TRUE;
            handle->current_state_id = BUTTON_EVENT_LONG_PRESS;

            if (handle->mode == BUTTON_MODE_INTERRUPT) {
                CFBDInputEvent event;
                CFBD_PlainButtonEventMaker(handle, &event);
                handle->base_inputs.context->ops->enqueue_event(handle->base_inputs.context,
                                                                &event);
            }
        }
    }

    if (handle->current_state_id == BUTTON_EVENT_LONG_PRESS && handle->is_pressed) {
        handle->current_state_id = BUTTON_EVENT_LONG_PRESS_HOLD;

        if (handle->mode == BUTTON_MODE_INTERRUPT) {
            CFBDInputEvent event;
            CFBD_PlainButtonEventMaker(handle, &event);
            handle->base_inputs.context->ops->enqueue_event(handle->base_inputs.context, &event);
        }
    }
}

void CFBD_PlainButtonSyncScan(CFBD_PlainButton* button)
{
    if (button->mode != BUTTON_MODE_POLLING) {
        return;
    }

    CFBD_Bool current_state = CFBD_GPIOReadPin(button->handle);
    uint32_t current_time = getApp(CFBD_FALSE)->tick_provider();
    CFBD_Bool is_pressed = (current_state == button->button_press_state);

    // 按下检测
    if (is_pressed && !button->is_pressed) {
        button->is_pressed = CFBD_TRUE;
        button->press_time = current_time;
        button->long_press_triggered = CFBD_FALSE;

        // 发送按下事件
        button->current_state_id = BUTTON_EVENT_PRESS;
        CFBDInputEvent event;
        CFBD_PlainButtonEventMaker(button, &event);
        button->base_inputs.context->ops->enqueue_event(button->base_inputs.context, &event);
    }

    // 释放检测
    if (!is_pressed && button->is_pressed) {
        button->is_pressed = CFBD_FALSE;
        button->release_time = current_time;
        uint32_t press_duration = button->release_time - button->press_time;

        // 发送释放事件
        button->current_state_id = BUTTON_EVENT_RELEASE;
        CFBDInputEvent event;
        CFBD_PlainButtonEventMaker(button, &event);
        button->base_inputs.context->ops->enqueue_event(button->base_inputs.context, &event);

        // 只在非长按的情况下处理点击/双击
        if (!button->long_press_triggered && press_duration < BUTTON_LONG_PRESS_TIME) {
            if (button->last_click_time != 0 &&
                (current_time - button->last_click_time) < BUTTON_DOUBLE_CLICK_TIME) {
                button->current_state_id = BUTTON_EVENT_DOUBLE_CLICK;
                button->last_click_time = 0; // 清零，避免三击被误判为双击

                CFBD_PlainButtonEventMaker(button, &event);
                button->base_inputs.context->ops->enqueue_event(button->base_inputs.context,
                                                                &event);
                return;
            }
            else {
                button->last_click_time = current_time;
            }
        }
        else {
            button->last_click_time = 0;
        }
    }

    // 长按检测（仅在按下状态）
    if (button->is_pressed) {
        ButtonOfLongSession(button);
    }
    else if (button->last_click_time != 0 &&
             (current_time - button->last_click_time) >= BUTTON_DOUBLE_CLICK_TIME) {
        button->current_state_id = BUTTON_EVENT_CLICK;
        button->last_click_time = 0;

        CFBDInputEvent event;
        CFBD_PlainButtonEventMaker(button, &event);
        button->base_inputs.context->ops->enqueue_event(button->base_inputs.context, &event);
    }
    else if (!button->is_pressed && button->last_click_time == 0) {
        button->current_state_id = BUTTON_EVENT_IDLE;
    }
}

void CFBD_PlainButtonEventMaker(CFBD_PlainButton* button, CFBDInputEvent* blank_event)
{
    blank_event->event_indicates = button->current_state_id;
    blank_event->device_id = button->base_inputs.device_id;
    blank_event->event_private_data = NULL;
    return;
}

static IntrButtonArrayInfo* g_buttons;
void CFBD_SetMonitoringButtons(IntrButtonArrayInfo* buttons)
{
    g_buttons = buttons;
}

// in impl.h as requests

CFBD_ButtonIntrCallbacks CFBD_PlainButtonGetIntrCallbacks()
{
    if (!g_buttons) {
        return NULL;
    }

    return CFBD_PlainButtonGetIntrCallbacksImpl(g_buttons);
}