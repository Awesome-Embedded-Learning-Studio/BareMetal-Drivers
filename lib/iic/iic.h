/**
 * @file iic.h
 * @brief I2C (IIC) abstraction layer for CFBD drivers.
 *
 * @details
 * This header defines a lightweight, backend-pluggable I2C interface
 * used by drivers in the project (for example the OLED driver). The
 * abstraction exposes message-based transfers, device readiness checks,
 * error reporting and optional DMA hooks. Concrete backends implement
 * the `CFBD_I2COperations` table and populate `CFBD_I2CHandle`.
 *
 * @page iic_integration_page I2C Integration Guide
 * @brief How to Integrate I2C Devices with the CFBD I2C Abstraction
 *
 * @section iic_integration_overview Overview
 * The CFBD I2C abstraction provides a platform-independent interface for
 * I2C communication. Backend implementations (e.g., STM32) provide concrete
 * device drivers, while the abstraction layer ensures consistent behavior
 * across different hardware platforms.
 *
 * @section iic_architecture Architecture
 * The I2C system consists of:
 * - **Operations Table** (`CFBD_I2COperations`): Backend implementation hooks
 * - **I2C Handle** (`CFBD_I2CHandle`): Public interface + private backend state
 * - **Messages** (`CFBD_I2C_Message`): Protocol-level read/write operations
 * - **High-Level Helpers** (`CFBD_I2CRead`, `CFBD_I2CWrite`): Convenience functions
 *
 * @section iic_workflow Typical Usage Workflow
 *
 * @subsection iic_init Initialization
 * @code{.c}
 * // 1. Create I2C handle (typically done by backend platform code)
 * CFBD_I2CHandle i2c_bus = {
 *     .ops = &i2c_stm_operations,      // Backend operations table
 *     .private_handle = &i2c_hardware  // Platform-specific state
 * };
 *
 * // 2. Initialize the I2C bus
 * int status = CFBD_I2CInit(&i2c_bus);
 * if (status != I2C_OK) {
 *     // Handle initialization error
 * }
 * @endcode
 *
 * @subsection iic_transfer Message Transfer
 * @code{.c}
 * // Example: Read from OLED at address 0x3C
 * CFBD_I2C_Message msgs[2];
 * uint8_t reg_addr = 0x00;  // Register to read
 * uint8_t data_buffer[32];  // Read buffer
 *
 * // Message 1: Write register address
 * msgs[0].addr = 0x3C;
 * msgs[0].flags = 0;
 * msgs[0].len = 1;
 * msgs[0].buf = &reg_addr;
 *
 * // Message 2: Read data (uses I2C_M_RD flag)
 * msgs[1].addr = 0x3C;
 * msgs[1].flags = I2C_M_RD;
 * msgs[1].len = sizeof(data_buffer);
 * msgs[1].buf = data_buffer;
 *
 * // Execute combined write-read transaction
 * status = CFBD_I2CTransfer(&i2c_bus, msgs, 2, 1000);
 * if (status == I2C_OK) {
 *     // data_buffer now contains read data
 * }
 * @endcode
 *
 * @subsection iic_convenience Using Convenience Helpers
 * @code{.c}
 * CFBD_I2C_IORequestParams req = {
 *     .addr7 = 0x3C,
 *     .mem_addr = 0x10,        // Device internal address
 *     .mem_addr_size = 1,      // 1 byte address
 *     .data = buffer,
 *     .len = sizeof(buffer),
 *     .timeout_ms = 1000
 * };
 *
 * // Simple high-level read
 * status = CFBD_I2CRead(&i2c_bus, &req);
 * @endcode
 */

#pragma once
#include <stddef.h>
#include <stdint.h>

#include "iic_error.h"

/**
 * @def I2C_M_RD
 * @brief Flag indicating a read transfer (slave -> master).
 * @details
 * When set in a message's `flags` field, indicates that the message
 * direction is a read operation (master receives from slave).
 *
 * @par Example - Combined Write-Read Transfer
 * @code{.c}
 * // Many I2C devices require: write address, then read data
 * CFBD_I2C_Message msgs[2];
 * uint8_t cmd = 0x01;  // Command/register
 * uint8_t response[8];
 *
 * // Write command (no I2C_M_RD flag)
 * msgs[0].addr = 0x50;
 * msgs[0].flags = 0;
 * msgs[0].len = 1;
 * msgs[0].buf = &cmd;
 *
 * // Read response (with I2C_M_RD flag)
 * msgs[1].addr = 0x50;
 * msgs[1].flags = I2C_M_RD;  // Indicates read operation
 * msgs[1].len = sizeof(response);
 * msgs[1].buf = response;
 *
 * CFBD_I2CTransfer(bus, msgs, 2, 1000);
 * @endcode
 */
#define I2C_M_RD 0x0001 /* read data, from slave to master */

/**
 * @def I2C_M_NOSTART
 * @brief Flag to suppress START condition between messages.
 * @details
 * When set, prevents generation of a START condition before this message.
 * Used in specialized scenarios (rare) where messages should be combined
 * without intermediate START conditions.
 */
#define I2C_M_NOSTART 0x4000 /* no start between messages (rare) */

/**
 * @struct CFBD_I2C_Message
 * @brief Single I2C message description used for multi-message transfers.
 *
 * @details
 * Provides the address, flags, buffer pointer and length for one
 * message in a potentially multi-message transfer. The `addr` field
 * contains a 7-bit device address.
 *
 * @par Message Direction
 * The `flags` field determines message direction:
 * - **flags = 0**: Write (master -> slave)
 * - **flags = I2C_M_RD**: Read (slave -> master)
 *
 * @par Example - Multi-Message Transaction
 * @code{.c}
 * // Read register 0x20 from SSD1306 OLED display
 * CFBD_I2C_Message msgs[2];
 * uint8_t addr_byte = 0x20;
 * uint8_t reg_data[2];
 *
 * // Message 1: Send register address (write)
 * msgs[0].addr = 0x3C;       // OLED I2C address
 * msgs[0].flags = 0;         // Write operation
 * msgs[0].len = 1;
 * msgs[0].buf = &addr_byte;
 *
 * // Message 2: Read register contents
 * msgs[1].addr = 0x3C;
 * msgs[1].flags = I2C_M_RD;  // Read operation
 * msgs[1].len = 2;
 * msgs[1].buf = reg_data;
 *
 * // Execute both messages atomically
 * int status = CFBD_I2CTransfer(bus, msgs, 2, 1000);
 * @endcode
 */
typedef struct _CFBD_I2C_Message
{
    uint16_t addr;  /**< 7-bit address (0..0x7F) */
    uint16_t flags; /**< I2C Message Flags (see I2C_M_* macros) */
    uint16_t len;   /**< length of buffer */
    uint8_t* buf;   /**< pointer to data */
} CFBD_I2C_Message;

/* Async Callbacks */
/**
 * @typedef CFBD_I2C_AsyncCallback
 * @brief Signature for asynchronous I2C transfer completion callbacks.
 * @details
 * Called when an asynchronous I2C operation completes. Enables interrupt-driven
 * or DMA-based transfers without blocking the main application thread.
 *
 * @param status Result status code (I2C_OK or negative error code).
 * @param arg    User-provided argument passed through the async API.
 *
 * @par Example - Async Transfer with Callback
 * @code{.c}
 * struct TransferContext {
 *     uint8_t *data;
 *     size_t len;
 *     int error_count;
 * } ctx;
 *
 * void transfer_complete_callback(int status, void *arg) {
 *     struct TransferContext *ctx = (struct TransferContext *)arg;
 *
 *     if (status != I2C_OK) {
 *         ctx->error_count++;
 *         if (status == I2C_ERR_TIMEOUT) {
 *             // Timeout handling
 *             schedule_retry();
 *         }
 *     } else {
 *         // Transfer successful
 *         process_received_data(ctx->data, ctx->len);
 *     }
 * }
 *
 * // Initiate async transfer
 * ctx.data = buffer;
 * ctx.len = sizeof(buffer);
 * CFBD_I2CTransferAsync(bus, msgs, count, transfer_complete_callback, &ctx);
 * @endcode
 */
typedef void CFBD_I2C_AsyncCallback(int status, void* arg);

/**
 * @typedef CFBD_I2CHandle
 * @brief Opaque forward-declared type representing an I2C bus instance.
 * @details
 * Represents a single I2C bus in the system. The handle combines
 * backend-agnostic operations with platform-specific state.
 */
typedef struct _CFBD_I2CHandle CFBD_I2CHandle;

/**
 * @struct CFBD_I2COperations
 * @brief Function table that a backend must implement to drive an I2C bus.
 *
 * @details
 * Backends populate this table with pointers to concrete functions that
 * implement initialization, transfers and optional DMA operations.
 * This design enables pluggable I2C backends (STM32, ESP32, etc.)
 * while maintaining a unified public interface.
 *
 * @defgroup CFBD_IIC_Backend I2C Backend Implementation
 * @ingroup cfbd_io
 * @brief Backend implementation interface for I2C drivers
 * @details
 * Device-specific implementations must provide a CFBD_I2COperations
 * table and initialize CFBD_I2CHandle with appropriate values.
 * @{
 */
typedef struct
{
    /**
     * @brief Initialize the I2C hardware/peripheral.
     * @details
     * Called once during system startup to configure clock, pins, and
     * I2C peripheral registers.
     *
     * @return I2C_OK on success, or negative error code on failure
     */
    int (*init)(CFBD_I2CHandle* handle);

    /**
     * @brief Deinitialize the I2C hardware/peripheral.
     * @details
     * Clean shutdown: disables interrupts, releases DMA, powers down hardware.
     *
     * @return I2C_OK on success, or negative error code on failure
     */
    int (*deinit)(CFBD_I2CHandle* handle);

    /**
     * @brief Perform a sequence of I2C messages.
     * @details
     * Core transfer function. Must support multiple messages in a single
     * transaction (write-read patterns, repeated START, etc.).
     *
     * @param bus I2C bus handle
     * @param msgs Array of messages to transfer
     * @param num Number of messages
     * @param timeout_ms Operation timeout in milliseconds
     * @return I2C_OK on success, or negative error code
     */
    int (*transfer)(CFBD_I2CHandle* bus, CFBD_I2C_Message* msgs, int num, uint32_t timeout_ms);

    /**
     * @brief Check whether a device is ready (acknowledges) within trials.
     * @details
     * Probes device presence by attempting multiple transactions.
     * Useful for detecting device hotplug, power-on sequences, etc.
     *
     * @param bus I2C bus handle
     * @param addr Device address (7-bit)
     * @param trials Number of probe attempts
     * @param timeout_ms Per-trial timeout
     * @return I2C_OK if device acknowledged, negative error otherwise
     */
    int (*is_device_ready)(CFBD_I2CHandle* bus,
                           uint16_t addr,
                           uint32_t trials,
                           uint32_t timeout_ms);

    /**
     * @brief Attempt to recover the I2C bus (clock recovery, toggling lines).
     * @details
     * Handles stuck bus scenarios by toggling SCL, sending dummy clocks,
     * or performing platform-specific recovery procedures.
     *
     * @return I2C_OK if recovered, negative error if recovery failed
     */
    int (*recover_bus)(CFBD_I2CHandle* bus);

    /**
     * @brief Retrieve the last backend-specific error code.
     * @details
     * Returns additional error details beyond the standard error codes,
     * such as hardware-specific status registers or state flags.
     *
     * @return Backend-specific error code
     */
    int (*get_error)(CFBD_I2CHandle* bus);

    /* optional DMA hooks */
    /**
     * @brief Initiate DMA transmission (optional).
     * @details
     * Enables efficient bulk data transmission via DMA.
     * Can return I2C_ERR_INVAL if DMA not supported.
     *
     * @return I2C_OK if DMA started, error code otherwise
     */
    int (*tx_dma_start)(CFBD_I2CHandle* bus, const uint8_t* buf, size_t len);

    /**
     * @brief Initiate DMA reception (optional).
     * @details
     * Enables efficient bulk data reception via DMA.
     *
     * @return I2C_OK if DMA started, error code otherwise
     */
    int (*rx_dma_start)(CFBD_I2CHandle* bus, uint8_t* buf, size_t len);
} CFBD_I2COperations;

/** @} */

/**
 * @typedef CFBD_I2CPrivateHandle
 * @brief Opaque pointer for backend-private state stored in the handle.
 * @details
 * Backend implementations use this to store any hardware-specific data,
 * such as peripheral base addresses, interrupt handlers, DMA channel info, etc.
 */
typedef void* CFBD_I2CPrivateHandle;

/**
 * @struct CFBD_I2CHandle
 * @brief Public I2C handle containing the operations table and private state.
 * @details
 * Represents a complete I2C bus instance combining platform-independent
 * operations with platform-specific state.
 *
 * @par Example - Initializing an I2C Handle
 * @code{.c}
 * // Backend (e.g., STM32) provides its operations
 * extern const CFBD_I2COperations stm32_i2c_ops;
 * extern void *i2c1_private_state;
 *
 * // Initialize handle
 * CFBD_I2CHandle bus = {
 *     .ops = &stm32_i2c_ops,
 *     .private_handle = i2c1_private_state
 * };
 *
 * // Now ready for use
 * CFBD_I2CInit(&bus);
 * @endcode
 */
typedef struct _CFBD_I2CHandle
{
    const CFBD_I2COperations* ops;        /**< Backend operation table. */
    CFBD_I2CPrivateHandle private_handle; /**< Backend-specific state. */
} CFBD_I2CHandle;

/* --------- inline wrappers  ---------- */
/**
 * @defgroup CFBD_IIC_Wrappers I2C Inline Wrappers
 * @brief Convenience inline functions for I2C operations
 * @details
 * These inline functions wrap backend operations, adding basic validation
 * and NULL-safety checks before delegating to the backend.
 * @ingroup cfbd_io
 * @{
 */

/**
 * @brief Inline helper to initialize an I2C bus using the backend.
 *
 * @param bus Pointer to I2C handle
 * @return int I2C_OK on success or a negative error code.
 *
 * @par Example
 * @code{.c}
 * CFBD_I2CHandle bus = get_i2c_bus();
 * if (CFBD_I2CInit(&bus) != I2C_OK) {
 *     handle_init_error();
 * }
 * @endcode
 */
static inline int CFBD_I2CInit(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->ops || !bus->ops->init)
        return I2C_ERR_INVAL;
    return bus->ops->init(bus);
}

/**
 * @brief Inline helper to deinitialize an I2C bus.
 *
 * @param bus Pointer to I2C handle
 * @return int I2C_OK on success or a negative error code.
 */
static inline int CFBD_I2CDeInit(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->ops || !bus->ops->deinit)
        return I2C_ERR_INVAL;
    return bus->ops->deinit(bus);
}

/**
 * @brief Inline wrapper for message transfers.
 *
 * @param bus I2C bus handle
 * @param msgs Array of I2C messages
 * @param num Number of messages in array
 * @param timeout_ms Operation timeout in milliseconds
 * @return int I2C_OK on success or a negative error code.
 *
 * @par Example - Simple Write
 * @code{.c}
 * uint8_t cmd[] = { 0x01, 0x02, 0x03 };
 * CFBD_I2C_Message msg = {
 *     .addr = 0x50,
 *     .flags = 0,  // Write
 *     .len = sizeof(cmd),
 *     .buf = cmd
 * };
 *
 * int status = CFBD_I2CTransfer(bus, &msg, 1, 1000);
 * @endcode
 */
static inline int
CFBD_I2CTransfer(CFBD_I2CHandle* bus, CFBD_I2C_Message* msgs, int num, uint32_t timeout_ms)
{
    if (!bus || !bus->ops || !bus->ops->transfer)
        return I2C_ERR_INVAL;
    return bus->ops->transfer(bus, msgs, num, timeout_ms);
}

/**
 * @brief Check if a device is ready using the backend provider.
 *
 * @param bus I2C bus handle
 * @param addr 7-bit device address
 * @param trials Number of probe attempts
 * @param timeout_ms Per-trial timeout in milliseconds
 * @return int I2C_OK if device ready, negative error code otherwise.
 *
 * @par Example - Device Presence Check
 * @code{.c}
 * // Wait for device to become ready (useful after power-on)
 * int status = CFBD_I2CTransferIsDeviceReady(bus, 0x3C, 10, 100);
 * if (status == I2C_OK) {
 *     // Device is present and responding
 * } else if (status == I2C_ERR_NACK) {
 *     // Device not responding
 * }
 * @endcode
 */
static inline int CFBD_I2CTransferIsDeviceReady(CFBD_I2CHandle* bus,
                                                uint16_t addr,
                                                uint32_t trials,
                                                uint32_t timeout_ms)
{
    if (!bus || !bus->ops || !bus->ops->is_device_ready)
        return I2C_ERR_INVAL;
    return bus->ops->is_device_ready(bus, addr, trials, timeout_ms);
}

/**
 * @brief Attempt bus recovery using backend hook.
 *
 * @param bus I2C bus handle
 * @return int I2C_OK if recovery successful, error code if failed.
 *
 * @details
 * Call this when transfers are failing persistently. Recovery typically
 * involves toggling SCL, sending dummy clocks, or issuing STOP conditions.
 *
 * @par Example - Stuck Bus Recovery
 * @code{.c}
 * int retry_count = 0;
 * int status;
 *
 * do {
 *     status = CFBD_I2CTransfer(bus, msgs, count, 100);
 *     if (status == I2C_OK) {
 *         break;
 *     }
 *
 *     if (status == I2C_ERR_TIMEOUT || status == I2C_ERR_IO) {
 *         CFBD_I2CRecoverBus(bus);  // Attempt recovery
 *         delay_ms(50);
 *     }
 * } while (++retry_count < 3);
 * @endcode
 */
static inline int CFBD_I2CRecoverBus(CFBD_I2CHandle* bus)
{
    if (!bus || !bus->ops || !bus->ops->recover_bus)
        return I2C_ERR_INVAL;
    return bus->ops->recover_bus(bus);
}

/** @} */

/**
 * @struct CFBD_I2C_IORequestParams
 * @brief Helper structure used by convenience read/write helpers.
 * @details
 * Simplifies high-level I2C read/write operations by bundling common
 * parameters (address, register offset, buffer, timeout) into a single
 * structure.
 *
 * @par Example - Register Read
 * @code{.c}
 * uint8_t register_value[4];
 * CFBD_I2C_IORequestParams req = {
 *     .addr7 = 0x50,           // Device address
 *     .mem_addr = 0x0100,      // Register/memory address in device
 *     .mem_addr_size = 2,      // 2-byte register address
 *     .data = register_value,
 *     .len = sizeof(register_value),
 *     .timeout_ms = 1000
 * };
 *
 * if (CFBD_I2CRead(bus, &req) == I2C_OK) {
 *     // register_value now contains data from address 0x0100
 * }
 * @endcode
 */
typedef struct
{
    uint16_t addr7;        /**< 7-bit I2C address. */
    uint32_t mem_addr;     /**< Optional device memory/register address. */
    uint8_t mem_addr_size; /**< Size in bytes of `mem_addr`. */
    uint8_t* data;         /**< Pointer to data buffer. */
    uint16_t len;          /**< Length of `data` in bytes. */
    uint32_t timeout_ms;   /**< Operation timeout in milliseconds. */
} CFBD_I2C_IORequestParams;

/**
 * @defgroup CFBD_IIC_HighLevel I2C High-Level Helpers
 * @brief Convenience functions for common I2C operations
 * @details
 * Simplify typical read/write patterns with automatic message construction.
 * @ingroup cfbd_io
 * @{
 */

/**
 * @brief High-level helper to read from an I2C device using IORequestParams.
 *
 * @param handle Pointer to I2C bus handle
 * @param r      Pointer to IORequestParams describing the read operation
 * @return int I2C_OK on success or negative error code.
 *
 * @details
 * Automatically constructs a read transaction that:
 * 1. Writes the device memory/register address to the device
 * 2. Reads the requested data from that address
 *
 * @par Example - EEPROM Read
 * @code{.c}
 * uint8_t buffer[16];
 * CFBD_I2C_IORequestParams req = {
 *     .addr7 = 0x50,          // EEPROM address
 *     .mem_addr = 0x200,      // Byte offset in EEPROM
 *     .mem_addr_size = 2,     // 16-bit address
 *     .data = buffer,
 *     .len = 16,
 *     .timeout_ms = 1000
 * };
 *
 * if (CFBD_I2CRead(bus_handle, &req) == I2C_OK) {
 *     // Process 16 bytes from EEPROM address 0x200
 * }
 * @endcode
 */
int CFBD_I2CRead(CFBD_I2CHandle* handle, CFBD_I2C_IORequestParams* r);

/**
 * @brief High-level helper to write to an I2C device using IORequestParams.
 *
 * @param handle Pointer to I2C bus handle
 * @param r      Pointer to IORequestParams describing the write operation
 * @return int I2C_OK on success or negative error code.
 *
 * @details
 * Constructs a write transaction that sends both the device memory/register
 * address and data in a single I2C transaction.
 *
 * @par Example - Configuration Write
 * @code{.c}
 * uint8_t config_data[] = { 0xAA, 0xBB, 0xCC };
 * CFBD_I2C_IORequestParams req = {
 *     .addr7 = 0x30,
 *     .mem_addr = 0x10,       // Register offset
 *     .mem_addr_size = 1,
 *     .data = config_data,
 *     .len = sizeof(config_data),
 *     .timeout_ms = 500
 * };
 *
 * int status = CFBD_I2CWrite(bus_handle, &req);
 * @endcode
 */
int CFBD_I2CWrite(CFBD_I2CHandle* handle, CFBD_I2C_IORequestParams* r);

/** @} */

#include "lib_settings.h"
#if defined(CFBD_IS_ST)
#include "backend/i2c_stm_impl.h"
#else
#error "No supports chips"
#endif