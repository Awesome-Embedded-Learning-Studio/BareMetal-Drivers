#pragma once

/**
 * @file ssd1306.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief import the func to init ssd1309 specifics
 * @version 0.1
 * @date 2026-01-02
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include "driver/device/device_interface.h"

#define SSD1309_DRIVER_ADDRESS (0x78)

CFBD_OLED_DeviceSpecific* getSSD1309Specific();
