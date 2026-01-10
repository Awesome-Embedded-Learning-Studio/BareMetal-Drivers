/**
 * @file external_impl_driver.h
 * @brief External (platform/application) implementation hooks for OLED driver.
 *
 * @details
 * This header defines structures used to pass platform-specific
 * initialization data to the OLED driver. It references the project's
 * I2C/IIC abstraction and the device interface declarations. The
 * structures here are intended to be filled by the board/application
 * code and then passed to the OLED driver during initialization.
 *
 * @page oled_integration_page OLED Driver Integration
 * @brief Platform Integration and Initialization for OLED Displays
 *
 * @section oled_integration_intro Introduction
 * This module facilitates platform-specific initialization of OLED displays
 * by providing a standardized initialization parameter structure. It decouples
 * the generic OLED driver from platform-specific details (I2C bus selection,
 * display models, timing parameters, etc.).
 *
 * @section oled_integration_flow Integration Flow
 *
 * @subsection oled_flow_platform Platform Code (board/system initialization)
 * 1. Initialize the I2C bus using platform-specific code
 * 2. Create a `CFBD_OLED_IICInitsParams` structure with:
 *    - I2C bus handle
 *    - Device address (0x3C or 0x3D for typical OLED displays)
 *    - Device-specific callbacks and configuration
 *    - Optional I2C callback for transaction status
 * 3. Call OLED driver's init function with the parameters
 *
 * @subsection oled_flow_example Typical Platform Initialization
 * @code{.c}
 * // In platform/board initialization code
 *
 * // Step 1: I2C bus is already initialized
 * extern CFBD_I2CHandle i2c1_bus;
 *
 * // Step 2: Define OLED device specifics (e.g., SSD1306)
 * extern CFBD_OLED_DeviceSpecific ssd1306_specs;
 *
 * // Step 3: Create initialization parameters
 * CFBD_OLED_IICInitsParams oled_params = {
 *     .i2cHandle = &i2c1_bus,
 *     .accepted_time_delay = 1,                // 1 tick delay
 *     .device_address = 0x3C,                  // SSD1306 default
 *     .device_specifics = &ssd1306_specs,
 *     .iic_transition_callback = my_iic_handler
 * };
 *
 * // Step 4: Initialize OLED
 * int status = CFBD_OLED_Init(&oled_params);
 * if (status != OLED_OK) {
 *     // Handle initialization error
 *     handle_error();
 * }
 * @endcode
 *
 * @section oled_integration_devices Supported Device Models
 * The framework supports multiple OLED controller models through the
 * `CFBD_OLED_DeviceSpecific` structure:
 * - **SSD1306**: 128×64 I2C OLED displays
 * - **SSD1309**: Enhanced variant with higher speeds
 * - **SH1106**: Alternative controller (less common)
 *
 * @section oled_integration_callbacks Callback Pattern
 * The I2C transition callback enables asynchronous notification when
 * I2C transactions complete:
 *
 * @code{.c}
 * void my_iic_handler(int status) {
 *     if (status == I2C_OK) {
 *         // I2C transaction succeeded
 *         signal_display_updated();
 *     } else if (status == I2C_ERR_NACK) {
 *         // Display not responding
 *         log_error("Display not found on I2C bus");
 *     } else if (status == I2C_ERR_TIMEOUT) {
 *         // Transaction timeout
 *         log_error("Display I2C timeout");
 *     }
 * }
 * @endcode
 *
 * @section oled_integration_timing Timing Configuration
 * The `accepted_time_delay` parameter controls inter-transaction delays.
 * This is useful for:
 * - Allowing display controller time to process commands
 * - Pacing rapid consecutive updates
 * - Accommodating system scheduler granularity
 */

#pragma once

#include "driver/device/device_interface.h"

/* I2C/IIC backend (points to the project I2C driver) */
#include "../iic/iic.h"

/**
 * @defgroup CFBD_OLED_Integration OLED Display Integration
 * @brief OLED driver initialization and platform integration
 * @ingroup cfbd_display
 * @details
 * Provides structures and patterns for integrating OLED displays
 * with platform-specific I2C buses and device configurations.
 * @{
 */

/**
 * @struct CFBD_OLED_IICInitsParams
 * @brief Initialization parameters for OLED devices using I2C.
 *
 * @details
 * Aggregates the I2C handle, device address, timing parameters and a
 * pointer to device-specific data required by the OLED driver. The
 * optional `iic_transition_callback` can be supplied by the platform to
 * receive status updates about I2C transaction completion.
 *
 * @par Field Relationships
 * All fields must be properly initialized before passing to OLED_Init():
 * - `i2cHandle` must point to an initialized I2C bus
 * - `device_address` must match the actual hardware (0x3C or 0x3D typical)
 * - `device_specifics` must match the actual display model
 * - `iic_transition_callback` may be NULL if polling is acceptable
 *
 * @par Example - Complete Initialization
 * @code{.c}
 * // Create and configure the initialization structure
 * CFBD_OLED_IICInitsParams params;
 *
 * // Get I2C bus handle (configured by board layer)
 * params.i2cHandle = get_i2c1_handle();
 *
 * // Configure device-specific parameters
 * params.device_address = 0x3C;
 * params.accepted_time_delay = 5;           // 5 ticks between ops
 * params.device_specifics = get_ssd1306_config();
 * params.iic_transition_callback = on_iic_complete;
 *
 * // Initialize OLED
 * int result = OLED_Initialize(&params);
 * @endcode
 */
typedef struct
{
    /**
     * @brief Pointer to the I2C handle used for communication.
     *
     * This should point to a valid `CFBD_I2CHandle` provided by the
     * project's I2C driver (`lib/iic`). The handle must be already
     * initialized before passing to the OLED driver.
     *
     * @note The I2C handle remains owned by the board/platform layer.
     *       The OLED driver does not take ownership or close it.
     */
    CFBD_I2CHandle* i2cHandle;

    /**
     * @brief Minimum or accepted time delay (units implementation-defined).
     *
     * Used by the driver to schedule retries or throttling between
     * transactions. Interpret the unit according to the platform's
     * timing conventions (milliseconds, ticks, scheduler quantum, etc.).
     *
     * Typical values:
     * - **0-1**: No delay (fast polling systems)
     * - **1-5**: Minimal delay (cooperative multitasking)
     * - **5-10**: Small delay (allows other tasks to run)
     * - **10+**: Significant delay (low-priority display updates)
     *
     * @note The actual timing behavior depends on the OLED driver
     *       implementation and the platform's timer/scheduler.
     */
    uint32_t accepted_time_delay;

    /**
     * @brief 7-bit or 8-bit I2C device address depending on backend.
     *
     * Standard OLED display addresses:
     * - **0x3C**: Most common (SSD1306, SSD1309 default)
     * - **0x3D**: Alternative address (if SA0 pin tied high)
     *
     * @note The actual interpretation (7-bit vs 8-bit) is backend-specific.
     *       Consult the I2C driver documentation for address format.
     *
     * @par Example - Address Selection by Hardware
     * @code{.c}
     * // Check SA0 pin to determine address
     * #ifdef OLED_SA0_HIGH
     *     params.device_address = 0x3D;
     * #else
     *     params.device_address = 0x3C;
     * #endif
     * @endcode
     */
    uint16_t device_address;

    /**
     * @brief Pointer to device-specific configuration and callbacks.
     *
     * Implementation-defined structure describing device parameters
     * such as display geometry, command set quirks, or initialization
     * sequences. The concrete type is `CFBD_OLED_DeviceSpecific`.
     *
     * Device-specific data typically includes:
     * - Display resolution (width, height in pixels)
     * - Controller model (SSD1306, SSD1309, SH1106)
     * - Initialization command sequences
     * - Timing constants (frame rate, refresh rate)
     * - Feature flags (supports DMA, contrast adjustment, etc.)
     *
     * @note Must not be NULL. The OLED driver requires this to be
     *       properly initialized for the specific hardware model.
     *
     * @see CFBD_OLED_DeviceSpecific
     */
    CFBD_OLED_DeviceSpecific* device_specifics;

    /**
     * @brief Optional callback invoked on I2C transaction transitions.
     *
     * The callback receives an integer `status` describing the result
     * of a transaction (semantics are platform-defined). Use this to
     * signal completion or error conditions back to the application.
     *
     * Typical status values:
     * - **I2C_OK**: Transaction succeeded
     * - **I2C_ERR_NACK**: Device not responding
     * - **I2C_ERR_TIMEOUT**: Operation timed out
     * - **I2C_ERR_BUSY**: Bus busy, retry needed
     *
     * @par Callback Pattern - Async Notification
     * @code{.c}
     * void on_display_event(int status) {
     *     if (status == I2C_OK) {
     *         // Display command executed
     *         update_display_state();
     *     } else {
     *         // Display error
     *         increment_error_counter();
     *         schedule_retry();
     *     }
     * }
     *
     * params.iic_transition_callback = on_display_event;
     * @endcode
     *
     * @note May be NULL if the application does not require async
     *       notification. In that case, the driver may use polling
     *       or ignore completion status.
     *
     * @see CFBD_I2C_AsyncCallback
     */
    void (*iic_transition_callback)(int status);
} CFBD_OLED_IICInitsParams;

/** @} */
