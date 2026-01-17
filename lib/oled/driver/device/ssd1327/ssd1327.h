#pragma once
#include <stdint.h>

#include "driver/device/device_interface.h"

#define SSD1327_DRIVER_ADDRESS (0x78)
#define SSD1327_IIC_PACK (SSD132X_REQUEST_IIC_PACK)

CFBD_OLED_DeviceSpecific* getSSD1327Specific();
