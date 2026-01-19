#pragma once
#include <stdint.h>

#include "cfbd_define.h"
#include "input_config.h"
#include "input_device_base.h"
#include "input_event.h"

typedef CFBD_Bool (*CFBDEventInputHandler)(CFBDInputEvent* event);
typedef struct __CFBDInputEventContext CFBDInputEventContext;
typedef struct __CFBDInputEventContextOperations
{
    CFBD_Bool (*enqueue_event)(CFBDInputEventContext* context, CFBDInputEvent* event);
    CFBD_Bool (*sync_poll)(CFBDInputEventContext* context);
    CFBD_Bool (*register_handle)(CFBDInputEventContext* context, CFBDEventInputHandler handler);
    void (*unregister_handler)(CFBDInputEventContext* context, CFBDEventInputHandler handler);
} CFBDInputEventContextOperations;

typedef struct __CFBDInputEventContext
{
    CFBDInputEventContextOperations* ops;
    CFBDEventInputHandler handlers[INPUT_MAX_HANDLER_CNT];
    uint8_t handler_count;
    CFBDInputEvent queue[INPUT_EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;

    CFBD_Bool stop_propergate_if_failed;
} CFBDInputEventContext;

void CFBD_CreateInputContext(CFBDInputEventContext* context_init);
void CFBD_SetGlobalInputEventContext(CFBDInputEventContext* context);
const CFBDInputEventContext* CFBD_GetGlobalInputEventContext();

static inline void CFBD_CreateGlobalContext(CFBDInputEventContext* context_init)
{
    CFBD_CreateInputContext(context_init);
    CFBD_SetGlobalInputEventContext(context_init);
}