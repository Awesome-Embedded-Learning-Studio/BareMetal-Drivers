/**
 * @file ssd1309.h
 * @brief SSD1309 device-specific descriptor and factory.
 * @ingroup OLED_Device
 *
 * @details
 * This module provides the SSD1309-specific implementation of the device
 * descriptor interface. It delivers initialization sequences, GRAM buffers,
 * and communication parameters optimized for SSD1309-based displays.
 *
 * The SSD1309 is an enhanced monochrome OLED controller featuring:
 * - Extended resolution support (typically 128x64 or larger)
 * - Improved contrast and brightness control
 * - I2C and SPI communication interfaces
 * - Advanced timing parameters for various OLED panel types
 * - Fixed default I2C address: 0x78
 *
 * @see CFBD_OLED_DeviceSpecific for the generic interface
 * @see device_interface.h for base types
 */

#pragma once
#include "driver/device/device_interface.h"

/**
 * @def SSD1309_DRIVER_ADDRESS
 * @brief Default I2C address for SSD1309 devices.
 * @ingroup OLED_Device
 *
 * @details
 * The standard I2C address for SSD1309 OLED controllers is 0x78 (0111 1000).
 * This is the primary address used for single-device configurations.
 * Some modules may support alternative addresses via hardware configuration.
 *
 * @note This address is used when initializing I2C communication with the SSD1309 device.
 */
#define SSD1309_DRIVER_ADDRESS (0x78)

/**
 * @brief Factory function: returns the SSD1309 device-specific descriptor.
 *
 * @details
 * Retrieves a pointer to the static SSD1309 device descriptor. This descriptor
 * encapsulates all hardware-specific parameters needed to initialize and operate
 * an SSD1309 display via the generic OLED driver interface.
 *
 * The returned descriptor contains:
 * - Device-specific initialization command sequences
 * - Frame buffer (GRAM) pointers optimized for SSD1309 memory layout
 * - I2C/SPI communication prefixes with SSD1309-specific values
 * - Display resolution adapted for SSD1309 capabilities
 * - Enhanced timing and contrast parameters
 *
 * @return CFBD_OLED_DeviceSpecific* Pointer to SSD1309-specific data.
 *         The pointer is never NULL and must not be freed.
 *
 * @note
 * - This function is thread-safe and always returns the same descriptor.
 * - The returned data is read-only and should not be modified by callers.
 * - The default I2C address (SSD1309_DRIVER_ADDRESS) is embedded in the descriptor.
 *
 * @example
 * @code
 * // Example: Obtaining SSD1309 device descriptor for I2C initialization
 * #include "driver/device/ssd1309/ssd1309.h"
 * #include "driver/backend/oled_iic.h"
 * #include "oled.h"
 *
 * CFBD_OLED_DeviceSpecific* device = getSSD1309Specific();
 * CFBD_OLED oled_handle;
 *
 * // Setup I2C parameters for SSD1309
 * CFBD_OLED_IICInitsParams iic_params = {
 *     .device_address = SSD1309_DRIVER_ADDRESS,
 *     // ... other I2C parameters
 * };
 *
 * // Initialize OLED device
 * CFBD_GetOLEDHandle(&oled_handle, CFBD_OLEDDriverType_IIC, &iic_params, CFBD_TRUE);
 * @endcode
 *
 * @see getSSD1306Specific() for SSD1306 variant
 * @see SSD1309_DRIVER_ADDRESS for the default I2C address
 * @see CFBD_GetOLEDHandle() for driver initialization
 */
CFBD_OLED_DeviceSpecific* getSSD1309Specific();
