#pragma once

#include "driver/device/device_interface.h"

// iic
#include "../iic/iic.h" //< points to the IIC Driver

typedef struct
{
    CFBD_I2CHandle* i2cHandle;
    uint32_t accepted_time_delay;
    uint16_t device_address;

    CFBD_OLED_DeviceSpecific* device_specifics;
    void (*iic_transition_callback)(int status);
} CFBD_OLED_IICInitsParams;
