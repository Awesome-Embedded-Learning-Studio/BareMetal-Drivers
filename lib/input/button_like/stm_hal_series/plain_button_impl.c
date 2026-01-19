

#include "../config/lib_settings.h"

#if defined(CFBD_IS_ST)
#include "button_like/plain_button.h"
#include "button_like/plain_button_impl.h"
#include "cfbd_define.h"
#include "gpio-stm-impl.h"

static IntrButtonArrayInfo* g_info;

static void each_button_process(CFBD_PlainButton* button)
{
    GPIO_PinState pin_state = CFBD_GPIOReadPin(button->handle);
    uint32_t current_time = HAL_GetTick();

    if (button->click_pending_time > 0 &&
        (current_time - button->click_pending_time) >= BUTTON_DOUBLE_CLICK_TIME) {
        // 单击超时,确认为单击
        button->current_state_id = BUTTON_EVENT_CLICK;
        button->click_pending_time = 0;

        CFBDInputEvent event;
        CFBD_PlainButtonEventMaker(button, &event);
        button->base_inputs.context->ops->enqueue_event(button->base_inputs.context, &event);
    }

    // ========================================
    // 🔧 步骤3: 按钮状态处理
    // ========================================
    CFBD_Bool is_pressed = (pin_state == button->button_press_state);
    CFBDInputEvent event;
    CFBD_Bool should_send_event = CFBD_FALSE;

    // --- 按下事件 ---
    if (is_pressed && !button->is_pressed) {
        button->is_pressed = CFBD_TRUE;
        button->press_time = current_time;
        button->current_state_id = BUTTON_EVENT_PRESS;
        button->long_press_triggered = CFBD_FALSE;

        // 检查是否是双击的第二次按下
        if (button->click_pending_time > 0 &&
            (current_time - button->click_pending_time) < BUTTON_DOUBLE_CLICK_TIME) {
            // 这是双击!取消待定的单击
            button->is_double_click_pending = CFBD_TRUE;
            button->click_pending_time = 0; // 清除单击待定
        }

        should_send_event = CFBD_TRUE;
    }
    else if (!is_pressed && button->is_pressed) {
        button->is_pressed = CFBD_FALSE;
        button->release_time = current_time;
        uint32_t press_duration = current_time - button->press_time;

        // 非长按的短按
        if (!button->long_press_triggered && press_duration < BUTTON_LONG_PRESS_TIME) {
            // 检查是否是双击的第二次释放
            if (button->is_double_click_pending) {
                button->current_state_id = BUTTON_EVENT_DOUBLE_CLICK;
                button->is_double_click_pending = CFBD_FALSE;
                button->click_pending_time = 0;
                should_send_event = CFBD_TRUE;
            }
            else {
                button->current_state_id = BUTTON_EVENT_CLICK;
                button->click_pending_time = current_time;
                should_send_event = CFBD_FALSE;
            }
        }
        else {
            // 长按释放
            button->current_state_id = BUTTON_EVENT_RELEASE;
            button->click_pending_time = 0;
            button->is_double_click_pending = CFBD_FALSE;
            should_send_event = CFBD_TRUE;
        }
    }
    // --- 持续按下:长按检测 ---
    else if (button->is_pressed && !button->long_press_triggered) {
        uint32_t press_duration = current_time - button->press_time;

        if (press_duration >= BUTTON_LONG_PRESS_TIME) {
            // 触发长按
            button->long_press_triggered = CFBD_TRUE;
            button->current_state_id = BUTTON_EVENT_LONG_PRESS;
            button->click_pending_time = 0;
            should_send_event = CFBD_TRUE;
        }
    }

    // ========================================
    // 🔧 步骤4: 发送事件
    // ========================================
    if (should_send_event) {
        CFBD_PlainButtonEventMaker(button, &event);
        if (button->base_inputs.context && button->base_inputs.context->ops) {
            button->base_inputs.context->ops->enqueue_event(button->base_inputs.context, &event);
        }
    }
}

static void CFBD_STButtonIntrCallbacks(void* intr_datas)
{
    uint16_t GPIO_Pin = *(uint16_t*) intr_datas;
    for (int i = 0; i < g_info->cnt; i++) {
        CFBD_PlainButton* btn = g_info->array[i];
        if (btn->handle->pin_internals_ == GPIO_Pin) {
            each_button_process(btn);
        }
    }
}

CFBD_ButtonIntrCallbacks CFBD_PlainButtonGetIntrCallbacksImpl(IntrButtonArrayInfo* info)
{
    g_info = info;
    return CFBD_STButtonIntrCallbacks;
}

#endif