#include "input_device_base.h"

#include <stddef.h>

#include "cfbd_define.h"
#include "input.h"
#include "input_id_allocator.h"

static CFBD_Bool bind_context(CFBDInputDeviceBase* device, CFBDInputEventContext* new_context)
{
    device->context = new_context;
    return CFBD_TRUE;
}

static CFBD_Bool unbind_context(CFBDInputDeviceBase* device)
{
    device->context = NULL;
    return CFBD_TRUE;
}

static CFBD_Bool report_once(CFBDInputDeviceBase* device, CFBDInputEvent* event)
{
    if (!device->context) {
        return CFBD_FALSE;
    }

    return device->context->ops->enqueue_event(device->context, event);
}

static CFBDInputDeviceBaseOperation ops = {.bind_context = bind_context,
                                           .unbind_context = unbind_context,
                                           .report_once = report_once};

static CFBD_DeviceIDGenerator* allocator = NULL;

void setDeviceIDGenerator(CFBD_DeviceIDGenerator* id)
{
    allocator = id;
}

void CFBD_CreateInputDeviceBase(CFBDInputDeviceBase* base,
                                CFBDInputEventContext* context,
                                void* assigned_privates)
{
    base->device_internals = assigned_privates;
    base->ops = &ops;
    base->context = context;

    // allocate id
    if (!allocator) {
        allocator = CFBD_CreateDeviceIDDefaultGenerator();
    }

    base->device_id = allocator->get_next();
}