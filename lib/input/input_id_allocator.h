#pragma once
#include "input_device_base.h"
typedef struct __CFBD_DeviceIDGenerator
{
    CFBDInputDeviceID_t (*get_next)();
} CFBD_DeviceIDGenerator;

CFBD_DeviceIDGenerator* CFBD_CreateDeviceIDDefaultGenerator();
void CFBD_CreateDeviceIDCustomGenerator(CFBD_DeviceIDGenerator* gen,
                                        CFBDInputDeviceID_t (*get_next)());