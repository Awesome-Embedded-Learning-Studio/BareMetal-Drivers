#pragma once

/**
 * @file iic_error.h
 * @brief I2C/IIC error codes used by the CFBD I2C abstraction.
 *
 * @details
 * Small set of error code macros used across the I2C driver
 * infrastructure. Error codes mirror common POSIX/Linux errno-style
 * values to ease mapping to platform-level errors.
 *
 * @page iic_error_page I2C Error Handling
 * @brief Comprehensive Guide to I2C Error Codes and Recovery
 *
 * @section iic_error_intro Introduction
 * The I2C/IIC error subsystem provides a standardized set of error codes
 * compatible with POSIX conventions. This enables consistent error handling
 * across different platforms and I2C implementations within the CFBD project.
 *
 * @section iic_error_codes Error Code Reference
 * All I2C operations return an integer status code:
 * - **I2C_OK (0)**: Operation completed successfully
 * - **I2C_ERR_TIMEOUT (-110)**: Operation exceeded time limit
 * - **I2C_ERR_NACK (-121)**: Device did not acknowledge (protocol error)
 * - **I2C_ERR_BUSY (-16)**: Bus or device is currently busy
 * - **I2C_ERR_IO (-5)**: Generic I/O error occurred
 * - **I2C_ERR_INVAL (-22)**: Invalid arguments provided
 *
 * @section iic_error_handling Error Handling Patterns
 *
 * @subsection iic_error_basic Basic Error Checking
 * @code{.c}
 * int status = CFBD_I2CTransfer(bus, messages, num_messages, 1000);
 * if (status != I2C_OK) {
 *     // Handle error based on type
 *     if (status == I2C_ERR_TIMEOUT) {
 *         // Timeout occurred - retry or increase timeout
 *     } else if (status == I2C_ERR_NACK) {
 *         // Device not responding - check address and connections
 *     }
 * }
 * @endcode
 *
 * @subsection iic_error_recovery Recovery Strategies
 * @code{.c}
 * int attempt = 0;
 * const int MAX_RETRIES = 3;
 * int status;
 *
 * do {
 *     status = CFBD_I2CTransfer(bus, messages, num_messages, 1000);
 *     if (status == I2C_OK) {
 *         break;  // Success
 *     } else if (status == I2C_ERR_BUSY) {
 *         // Wait before retry
 *         delay_ms(10);
 *     } else if (status == I2C_ERR_NACK || status == I2C_ERR_IO) {
 *         // Attempt bus recovery
 *         CFBD_I2CRecoverBus(bus);
 *         delay_ms(50);
 *     }
 * } while (++attempt < MAX_RETRIES && status != I2C_OK);
 * @endcode
 */

/**
 * @defgroup CFBD_IIC_Error I2C/IIC Error Codes
 * @brief Standard error codes for I2C operations
 * @details
 * This module provides a set of standard error codes for I2C/IIC operations
 * in the CFBD driver suite. All error codes follow POSIX errno conventions.
 * @ingroup cfbd_io
 * @{
 */

/** @def I2C_OK
 * @brief Success return code.
 * @details
 * Returned when an I2C operation completes successfully without errors.
 * All subsequent device states are valid and the operation result (if any)
 * is reliable.
 *
 * @par Example
 * @code{.c}
 * int status = CFBD_I2CTransfer(bus, msg, count, timeout);
 * if (status == I2C_OK) {
 *     // Process received data
 *     process_data(msg[0].buf, msg[0].len);
 * }
 * @endcode
 */
#define I2C_OK 0

/** @def I2C_ERR_TIMEOUT
 * @brief Operation timed out (-ETIMEDOUT).
 * @details
 * The I2C operation did not complete within the specified timeout period.
 * This typically indicates:
 * - Clock line held low by slave (clock stretching exceeded timeout)
 * - No acknowledgment within expected time window
 * - Bus unresponsive for entire timeout duration
 *
 * @par Recovery Strategy
 * @code{.c}
 * if (status == I2C_ERR_TIMEOUT) {
 *     // Option 1: Attempt bus recovery
 *     CFBD_I2CRecoverBus(bus);
 *
 *     // Option 2: Retry with extended timeout
 *     delay_ms(50);
 *     status = CFBD_I2CTransfer(bus, msg, count, 2000);
 * }
 * @endcode
 */
#define I2C_ERR_TIMEOUT -110 /* -ETIMEDOUT */

/** @def I2C_ERR_NACK
 * @brief Protocol error / NACK received (-EPROTO).
 * @details
 * The addressed slave device did not acknowledge the address or data byte.
 * This indicates:
 * - Device not present at specified address
 * - Device in error state or not responding
 * - Address selection error
 *
 * @par Troubleshooting
 * @code{.c}
 * if (status == I2C_ERR_NACK) {
 *     // Check if device is ready
 *     status = CFBD_I2CTransferIsDeviceReady(bus, addr, 5, 100);
 *
 *     if (status != I2C_OK) {
 *         // Device not present or not responding
 *         return -1;  // Abort operation
 *     }
 * }
 * @endcode
 */
#define I2C_ERR_NACK -121 /* -EPROTO (nack) */

/** @def I2C_ERR_BUSY
 * @brief Bus or peripheral busy (-EBUSY).
 * @details
 * The I2C bus is currently occupied by another master or the peripheral
 * cannot accept new commands. This is typically a transient condition.
 *
 * @par Recommended Action
 * @code{.c}
 * if (status == I2C_ERR_BUSY) {
 *     // Small delay and retry
 *     delay_ms(1);
 *     status = CFBD_I2CTransfer(bus, msg, count, timeout);
 * }
 * @endcode
 */
#define I2C_ERR_BUSY -16 /* -EBUSY */

/** @def I2C_ERR_IO
 * @brief Generic I/O error (-EIO).
 * @details
 * A generic I/O error occurred during the I2C transaction. This may
 * indicate hardware malfunction, disconnected bus, or electrical issues.
 * The specific cause requires platform-specific debugging.
 */
#define I2C_ERR_IO -5 /* -EIO */

/** @def I2C_ERR_INVAL
 * @brief Invalid argument supplied (-EINVAL).
 * @details
 * Invalid parameters were passed to the I2C function. Common causes:
 * - NULL handle or message pointer
 * - Invalid device address (outside 7-bit range)
 * - Zero message count
 * - Invalid timeout value
 *
 * @par Validation Example
 * @code{.c}
 * if (addr > 0x7F) {
 *     return I2C_ERR_INVAL;  // 7-bit address out of range
 * }
 * if (num_messages <= 0 || !messages) {
 *     return I2C_ERR_INVAL;  // Invalid message array
 * }
 * @endcode
 */
#define I2C_ERR_INVAL -22 /* -EINVAL */

/** @} */
