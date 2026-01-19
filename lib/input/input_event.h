#pragma once
#include "cfbd_define.h"
#include "input_device_base.h"

typedef uint32_t CFBDInputDeviceEventType_t;
typedef struct __CFBDInputEventContext CFBDInputEventContext;

#define CFBDInputDeviceEventType_INVALID_ID (0)
#define CFBDInputDeviceEventType_SYSDEF_START (CFBDInputDeviceEventType_INVALID_ID + 1)
#define CFBDInputDeviceEventType_USER_START (0x100)

typedef struct __CFBDInputEvent
{
    CFBDInputDeviceEventType_t event_indicates; // what events?
    CFBDInputDeviceID_t device_id;              // what device?
    void* event_private_data;                   // event_datas
    CFBD_Bool stop_propergate;                  // should stop propergate?
} CFBDInputEvent; // OK, Events here contains what where are the session from

void CFBD_CreateEvent(CFBDInputEvent* event,
                      CFBDInputDeviceBase* baseDevic,
                      CFBDInputDeviceEventType_t eventType);

static inline void CFBD_SetEventStopHandled(CFBDInputEvent* event)
{
    event->stop_propergate = CFBD_TRUE;
}