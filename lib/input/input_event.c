#include "input_event.h"

void CFBD_CreateEvent(CFBDInputEvent* event,
                      CFBDInputDeviceBase* baseDevice,
                      CFBDInputDeviceEventType_t eventType)
{
    event->device_id = baseDevice->device_id;
    event->event_private_data = baseDevice->device_internals;
    event->event_indicates = eventType;
}