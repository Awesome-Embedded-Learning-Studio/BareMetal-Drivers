/**
 * @file ssd1306.h
 * @brief SSD1306 device-specific descriptor and factory.
 * @ingroup OLED_Device
 *
 * @details
 * This module provides the SSD1306-specific implementation of the device
 * descriptor interface. It delivers initialization sequences, GRAM buffers,
 * and communication parameters tailored for SSD1306-based displays.
 *
 * The SSD1306 is a common monochrome OLED controller featuring:
 * - 128x64 pixel resolution (typical)
 * - I2C or SPI communication interface
 * - 256-byte graphics RAM
 * - Multiplex and COM output configurations
 *
 * @see CFBD_OLED_DeviceSpecific for the generic interface
 * @see device_interface.h for base types
 */

#pragma once
#include "driver/device/device_interface.h"

/**
 * @brief Factory function: returns the SSD1306 device-specific descriptor.
 *
 * @details
 * Retrieves a pointer to the static SSD1306 device descriptor. This descriptor
 * encapsulates all hardware-specific parameters needed to initialize and operate
 * an SSD1306 display via the generic OLED driver interface.
 *
 * The returned descriptor contains:
 * - Initialization command sequences
 * - Frame buffer (GRAM) pointers
 * - I2C/SPI communication prefixes
 * - Display resolution (typically 128x64)
 *
 * @return CFBD_OLED_DeviceSpecific* Pointer to SSD1306-specific data.
 *         The pointer is never NULL and must not be freed.
 *
 * @note
 * This function is thread-safe and always returns the same descriptor.
 * The returned data is read-only and should not be modified by callers.
 *
 * @example
 * @code
 * // Example: Obtaining SSD1306 device descriptor for initialization
 * #include "driver/device/ssd1306/ssd1306.h"
 * #include "oled.h"
 *
 * CFBD_OLED_DeviceSpecific* device = getSSD1306Specific();
 * CFBD_OLED oled_handle;
 * CFBD_OLEDDriverType driver_type = CFBD_OLEDDriverType_IIC;
 *
 * // The device descriptor is passed implicitly through the driver initialization
 * CFBD_GetOLEDHandle(&oled_handle, driver_type, NULL, CFBD_TRUE);
 * @endcode
 *
 * @see getSSD1309Specific() for SSD1309 variant
 * @see CFBD_GetOLEDHandle() for driver initialization
 */
CFBD_OLED_DeviceSpecific* getSSD1306Specific();
