/**
 * @file device_interface.h
 * @brief Generic device descriptor used by OLED drivers.
 * @ingroup OLED_Device
 *
 * @details
 * This module encapsulates device-specific information required by the generic
 * OLED driver, including pointers to initialization tables, GRAM buffers and
 * logical geometry. Concrete device drivers (e.g. SSD1306/SSD1309) provide
 * instances of this structure.
 *
 * @see CFBD_OLED_DeviceSpecific for the main interface structure.
 */

#pragma once
#include <stdint.h>

/**
 * @defgroup OLED_Device OLED Device Interface
 * @ingroup OLED
 * @brief Device-specific descriptors and configurations for OLED drivers.
 *
 * @details
 * This module provides the generic device descriptor interface used by all
 * OLED drivers. It abstracts hardware-specific details into a standardized
 * structure that concrete device implementations (SSD1306, SSD1309, etc.)
 * populate with device-specific data.
 *
 * Device drivers must provide:
 * - Initialization command tables
 * - GRAM/frame buffer pointers
 * - Communication prefixes (for I2C data/command differentiation)
 * - Display geometry information
 *
 * @see ssd1306.h for SSD1306 implementation
 * @see ssd1309.h for SSD1309 implementation
 * @{
 */

/**
 * @struct CFBD_OLED_DeviceSpecific
 * @brief Device-specific configuration for an OLED panel.
 *
 * @details
 * Fields in this structure control how the generic OLED driver
 * initializes and communicates with a specific panel. Implementations
 * should populate `init_session_tables` and `grams` with pointers to
 * static data describing the initialization sequence and frame
 * buffers respectively.
 *
 * @note
 * Implementations must ensure that:
 * - init_session_tables() returns a valid pointer or NULL if no initialization is needed
 * - grams points to valid frame buffer pointers
 * - All size and geometry fields are correctly populated
 *
 * @example
 * @code
 * // Example: Getting device-specific descriptor and querying geometry
 * CFBD_OLED_DeviceSpecific* device_spec = getSSD1306Specific();
 * if (device_spec != NULL) {
 *     uint16_t width = device_spec->logic_width;
 *     uint16_t height = device_spec->logic_height;
 *     uint8_t* frame_buffer = device_spec->grams[0];
 * }
 * @endcode
 */
typedef struct
{
    /**
     * @brief Optional callback to return initialization command tables.
     *
     * The callback should allocate or return a pointer to a contiguous
     * block containing the device initialization sequence bytes. If
     * no initialization sequence is required, this may be NULL.
     */
    uint8_t* (*init_session_tables)(void);

    /**
     * @brief Array of pointers to GRAM/frame buffers used by the device.
     *
     * The driver may expect one or more pointers here depending on the
     * device's memory layout.
     */
    uint8_t** grams;

    /**
     * @brief Size (in bytes) of the initialization tables returned by
     *        `init_session_tables`.
     */
    uint16_t init_session_tables_sz;

    /**
     * @brief Byte prefix to use for data transmissions to the display.
     */
    uint8_t data_prefix;

    /**
     * @brief Byte prefix to use for command transmissions to the display.
     */
    uint8_t cmd_prefix;

    /**
     * @brief Logical width of the display in pixels.
     */
    uint16_t logic_width;

    /**
     * @brief Logical height of the display in pixels.
     */
    uint16_t logic_height;
} CFBD_OLED_DeviceSpecific;

/** @} */ // end of OLED_Device group
