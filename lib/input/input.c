#include "input.h"

#include <stddef.h>

#include "cfbd_define.h"
#include "input_config.h"
#include "input_device_base.h"

static CFBD_Bool enqueue_event(CFBDInputEventContext* context_init, CFBDInputEvent* event)
{
    uint8_t next = (context_init->tail + 1) % INPUT_EVENT_QUEUE_SIZE;
    if (next != context_init->head) {
        context_init->queue[context_init->tail] = *event;
        context_init->tail = next;
        return CFBD_TRUE;
    }

    return CFBD_FALSE;
}

static CFBD_Bool sync_poll(CFBDInputEventContext* context_init)
{
    while (context_init->head != context_init->tail) {
        CFBDInputEvent* event = &context_init->queue[context_init->head];

        for (uint8_t i = 0; i < context_init->handler_count; i++) {
            CFBD_Bool result = context_init->handlers[i](event);
            if (!result && context_init->stop_propergate_if_failed) {
                // stop as event processed failed as when
                // stop_propergate_if_failed == true
                break;
            }

            if (event->stop_propergate) {
                break; // OK, event shutdown, stop propergate
            }
        }

        context_init->head = (context_init->head + 1) % INPUT_EVENT_QUEUE_SIZE;
    }

    return CFBD_TRUE;
}

CFBD_Bool input_register_handler(CFBDInputEventContext* context_init, CFBDEventInputHandler handler)
{
    if (context_init->handler_count >= INPUT_MAX_HANDLER_CNT || handler == NULL) {
        return CFBD_FALSE;
    }

    for (uint8_t i = 0; i < context_init->handler_count; i++) {
        if (context_init->handlers[i] == handler) {
            return CFBD_TRUE;
        }
    }

    context_init->handlers[context_init->handler_count++] = handler;
    return CFBD_TRUE;
}

void input_unregister_handler(CFBDInputEventContext* context_init, CFBDEventInputHandler handler)
{
    for (uint8_t i = 0; i < context_init->handler_count; i++) {
        if (context_init->handlers[i] == handler) {
            for (uint8_t j = i; j < context_init->handler_count - 1; j++) {
                context_init->handlers[j] = context_init->handlers[j + 1];
            }
            context_init->handler_count--;
            break;
        }
    }
}

static CFBDInputEventContextOperations ops = {.enqueue_event = enqueue_event,
                                              .sync_poll = sync_poll,
                                              .register_handle = input_register_handler,
                                              .unregister_handler = input_unregister_handler};

void CFBD_CreateInputContext(CFBDInputEventContext* context_init)
{
    context_init->ops = &ops;
    context_init->handler_count = 0;
    context_init->head = 0;
    context_init->tail = 0;
    context_init->stop_propergate_if_failed = CFBD_FALSE;
}

static CFBDInputEventContext* global_context = NULL;

void CFBD_SetGlobalInputEventContext(CFBDInputEventContext* context)
{
    global_context = context;
}

const CFBDInputEventContext* CFBD_GetGlobalInputEventContext()
{
    return global_context;
}