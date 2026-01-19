#pragma once
#include "cfbd_define.h"
#include "gpio.h"
#include "input.h"
#include "input_device_base.h"
#include "input_event.h"

#define BUTTON_DEBOUNCE_TIME 20
#define BUTTON_LONG_PRESS_TIME 1000
#define BUTTON_DOUBLE_CLICK_TIME 300

#define MAX_HOLDABLE_BUTTON (3)

typedef enum
{
    BUTTON_EVENT_IDLE = CFBDInputDeviceEventType_INVALID_ID,
    BUTTON_EVENT_PRESS = CFBDInputDeviceEventType_SYSDEF_START, // 按下事件
    BUTTON_EVENT_RELEASE,                                       // 释放事件
    BUTTON_EVENT_CLICK,                                         // 单击事件
    BUTTON_EVENT_DOUBLE_CLICK,                                  // 双击事件
    BUTTON_EVENT_LONG_PRESS,                                    // 长按事件
    BUTTON_EVENT_LONG_PRESS_HOLD                                // 长按保持
} CFBD_PlainButtonEventID;

typedef enum
{
    BUTTON_MODE_POLLING = 0, // 轮询模式
    BUTTON_MODE_INTERRUPT    // 中断模式
} CFBD_PlainButtonMode;

typedef struct _CFBD_PlainButton
{
    CFBDInputDeviceBase base_inputs;
    CFBD_GPIOHandle* handle;
    CFBD_PlainButtonMode mode;
    CFBDEventInputHandler stateChangedCb;
    CFBD_PlainButtonEventID current_state_id;

    uint32_t press_time;      // 按下时刻
    uint32_t release_time;    // 释放时刻
    uint32_t last_click_time; // 上次单击时刻

    CFBD_Bool is_double_click_pending;
    uint32_t click_pending_time;

    CFBD_Bool is_pressed;           // 当前是否按下
    CFBD_Bool long_press_triggered; // 长按已触发标志
    CFBD_Bool button_press_state;   // when button presssed, at what status should be for gpio
} CFBD_PlainButton;

typedef struct
{
    CFBD_GPIOHandle* gpio;
    CFBDEventInputHandler callbacks;
    CFBDInputEventContext* input_context;
    CFBD_Bool button_press_state;
    CFBD_Bool isInterruptMode;
    void (*enable_intr)(CFBD_PlainButton* button);
} CFBD_ButtonCreationPack;

void CFBD_CreatePlainButton(CFBD_PlainButton* button, CFBD_ButtonCreationPack* pack);
void CFBD_PlainButtonEventMaker(CFBD_PlainButton* button, CFBDInputEvent* blank_event);
void CFBD_PlainButtonSyncScan(CFBD_PlainButton* button);

typedef void (*CFBD_ButtonIntrCallbacks)(void* intr_datas);
typedef CFBD_PlainButton** CFBDPlainButtonArray;
typedef struct
{
    CFBDPlainButtonArray array;
    int cnt;
} IntrButtonArrayInfo;

void CFBD_SetMonitoringButtons(IntrButtonArrayInfo* buttons);
CFBD_ButtonIntrCallbacks CFBD_PlainButtonGetIntrCallbacks();
