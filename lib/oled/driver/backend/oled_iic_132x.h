
/**
 * @file oled_iic.h
 * @brief I2C-backed OLED driver initialization interface.
 * @ingroup OLED_Backend
 *
 * @details
 * This module provides the initialization and configuration interface for
 * OLED devices communicating via I2C (IIC). It bridges the generic OLED
 * driver with platform-specific I2C implementations, handling address
 * configuration, transaction callbacks, and I2C handle binding.
 *
 * Platform/application code should:
 * 1. Construct a CFBD_OLED_IICInitsParams structure with I2C parameters
 * 2. Create a CFBD_OLED instance
 * 3. Call CFBD_OLED_IICInit() to wire I2C backend to the generic OLED driver
 *
 * @see external_impl_driver.h for CFBD_OLED_IICInitsParams definition
 * @see oled.h for CFBD_OLED type
 */

#pragma once
#include "configs/external_impl_driver.h"
#include "oled.h"

/**
 * @defgroup OLED_Backend OLED Backend Implementations
 * @ingroup OLED
 * @brief Backend transport implementations (I2C, SPI) for OLED drivers.
 *
 * @details
 * This group contains transport-specific implementations for communicating
 * with OLED devices. Each backend (I2C, SPI) provides initialization routines
 * to wire transport parameters with the generic OLED driver.
 *
 * Backends must handle:
 * - Device address/chip-select configuration
 * - Transport handle binding
 * - Asynchronous transaction notifications
 * - Error handling and retry logic
 * @{
 */

/**
 * @brief Initialize an I2C-based OLED device instance.
 *
 * @details
 * Performs the necessary wiring between the generic CFBD_OLED object
 * and the platform-specific I2C implementation described by pvt_handle.
 * This function configures the device address, binds the I2C handle, and
 * sets up callbacks for asynchronous transaction notifications.
 *
 * After calling this function, the OLED object is ready for device
 * operations. The caller must ensure that the I2C interface is properly
 * initialized before calling this function.
 *
 * @param handle Pointer to the CFBD_OLED instance to initialize.
 *               Must point to a valid CFBD_OLED structure.
 * @param pvt_handle Pointer to CFBD_OLED_IICInitsParams containing
 *                   platform-specific I2C parameters.
 *                   Must be non-NULL and remain valid during the call.
 *
 * @note
 * - The OLED handle must not be used before initialization via CFBD_OLED_IICInit()
 * - pvt_handle parameters include device address, I2C bus handle, and callbacks
 * - This is a synchronous operation; no I2C communication occurs here
 * - Multiple OLED instances can be initialized with different addresses
 *
 * @example
 * @code
 * // Example: Complete I2C OLED initialization flow
 * #include "driver/backend/oled_iic.h"
 * #include "driver/device/ssd1309/ssd1309.h"
 *
 * // Step 1: Prepare I2C parameters
 * CFBD_OLED_IICInitsParams iic_params = {
 *     .device_address = SSD1309_DRIVER_ADDRESS,
 *     .i2c_handle = my_i2c_interface,
 *     .notify_tx_complete = on_transaction_complete,
 *     .notify_error = on_i2c_error
 * };
 *
 * // Step 2: Create and initialize OLED instance
 * CFBD_OLED oled_device;
 * CFBD_OLED_IICInit(&oled_device, &iic_params);
 *
 * // Step 3: Open the device and begin operations
 * if (oled_device.ops->open(&oled_device)) {
 *     // Device is ready for use
 *     oled_device.ops->update(&oled_device);
 * }
 * @endcode
 *
 * @see CFBD_OLED_IICInitsParams in external_impl_driver.h
 * @see CFBD_OLED for the device structure
 * @see ssd1309.h for device-specific initialization
 */
void CFBD_OLED_IIC132XInit(CFBD_OLED* handle, CFBD_OLED_IICInitsParams* pvt_handle);

/** @} */ // end of OLED_Backend group
