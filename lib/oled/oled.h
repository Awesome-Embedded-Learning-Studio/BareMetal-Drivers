/**
 * @file oled.h
 * @brief Generic OLED object and operations exposed to application code.
 * @defgroup OLED OLED Display Driver
 *
 * @details
 * This module defines the core OLED subsystem API: the generic CFBD_OLED object,
 * operation table (CFBD_OLEDOperations), and supporting types used throughout
 * the driver ecosystem. Concrete device drivers (SSD1306, SSD1309, etc.)
 * implement and populate the operation table to provide a uniform API to
 * application code.
 *
 * The module provides:
 * - Device abstraction via CFBD_OLED structure
 * - Operation tables for rendering, updates, and device management
 * - Transport-agnostic driver type selection (I2C or SPI)
 * - Query interface for device capabilities
 * - Area-based and frame-based operations
 *
 * Application code should interact primarily through:
 * - CFBD_GetOLEDHandle() for device initialization
 * - Operations in the ops table for rendering (setPixel, setArea, update, etc.)
 * - Query operations for device properties (resolution, RGB support)
 *
 * @version 0.1
 * @date 2026-01-09
 *
 * @see driver/device/device_interface.h for device descriptor interface
 * @see driver/backend/oled_iic.h for I2C backend initialization
 * @see driver/device/ssd1306/ssd1306.h for SSD1306 implementation
 * @see driver/device/ssd1309/ssd1309.h for SSD1309 implementation
 */

#pragma once
#include "cfbd_define.h"

/** @addtogroup OLED @{ */

/**
 * @enum CFBD_OLEDDriverType
 * @brief Transport/driver type used to communicate with the OLED device.
 *
 * @details
 * Specifies the underlying communication protocol used to interface with the
 * OLED controller. Different transport types may have different initialization
 * parameters and performance characteristics.
 *
 * @see CFBD_GetOLEDHandle() for usage when creating device handles
 */
typedef enum
{
    CFBD_OLEDDriverType_IIC, /**< I2C (IIC) transport - suitable for displays with I2C interface */
    CFBD_OLEDDriverType_SPI  /**< SPI transport - suitable for displays with SPI interface */
} CFBD_OLEDDriverType;

/**
 * @typedef CFBD_OLEDHandle
 * @brief Opaque handle type referencing driver-specific state.
 */
typedef void* CFBD_OLEDHandle;

/**
 * @typedef CFBD_OLED
 * @brief Forward declaration for the OLED object.
 */
typedef struct _CFBD_OLED CFBD_OLED;

/**
 * @typedef AreaOperations
 * @brief Function pointer type for area-based display operations.
 *
 * @details
 * This function pointer signature is used for operations that target a
 * rectangular area of the display (e.g., update_area, clear_area, revert_area).
 * Implementations should use clipping to handle areas that extend beyond
 * display boundaries.
 *
 * @param oled Pointer to the CFBD_OLED instance being operated on.
 * @param x X coordinate of the area's top-left corner (origin 0,0 at top-left).
 * @param y Y coordinate of the area's top-left corner.
 * @param width Width of the area in pixels. Combined with x must not exceed display width.
 * @param height Height of the area in pixels. Combined with y must not exceed display height.
 *
 * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
 *         Failures may include invalid coordinates or driver errors.
 *
 * @see CFBD_OLEDOperations for operation table definition
 * @example
 * @code
 * // Example: Clear a rectangular area at (10, 20) with size 64x32
 * CFBD_OLED* oled = &my_oled_device;
 * CFBD_Bool result = oled->ops->clear_area(oled, 10, 20, 64, 32);
 * if (result) {
 *     oled->ops->update_area(oled, 10, 20, 64, 32);
 * }
 * @endcode
 */
typedef CFBD_Bool (
        *AreaOperations)(CFBD_OLED* oled, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/**B
 * @typedef FrameOperation
 * @brief Function pointer type for full-frame display operations.
 *
 * @details
 * This function pointer signature handles operations that affect the entire
 * display frame (update, clear, revert operations). Typically used to push
 * local buffer changes to the display, clear all pixels, or restore the
 * previous frame content if supported by the device.
 *
 * @param oled Pointer to the CFBD_OLED instance being operated on.
 * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
 *
 * @see CFBD_OLEDOperations for operation table definition
 */
typedef CFBD_Bool (*FrameOperation)(CFBD_OLED* oled);

/**
 * @typedef OLEDSelfOperation
 * @brief Function pointer type for device lifecycle operations.
 *
 * @details
 * This function pointer signature is used for operations that manage the
 * device lifecycle (open/close/enable/disable). These operations typically
 * handle resource allocation, hardware initialization, and cleanup.
 *
 * @param oled Pointer to the CFBD_OLED instance being operated on.
 * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
 *
 * @see CFBD_OLEDOperations for operation table definition
 */
typedef CFBD_Bool (*OLEDSelfOperation)(CFBD_OLED* oled);

/**
 * @typedef OLED_QueryOperation
 * @brief Function pointer type for querying device-specific properties.
 *
 * @details
 * This function pointer signature handles runtime queries for device
 * capabilities and properties. Implementations must support standard
 * properties ("width", "height", "rgb") and may support additional
 * device-specific properties.
 *
 * Supported standard properties:
 * - "width": returns uint16_t display width in pixels
 * - "height": returns uint16_t display height in pixels
 * - "rgb": returns CFBD_Bool indicating RGB vs monochrome support
 *
 * @param oled Pointer to the CFBD_OLED instance being queried.
 * @param property Null-terminated string naming the queried property.
 * @param args Optional input arguments for the query (implementation-defined).
 *             Pass NULL if the query doesn't require additional arguments.
 * @param request_data Output pointer where query results are written.
 *                     Caller must allocate appropriate buffer based on property type.
 *
 * @return CFBD_Bool CFBD_TRUE on success (property found and returned),
 *                   CFBD_FALSE on failure (unknown property or error).
 *
 * @example
 * @code
 * // Example: Query display resolution
 * CFBD_OLED* oled = &my_oled_device;
 * uint16_t width = 0, height = 0;
 *
 * oled->ops->self_consult(oled, "width", NULL, &width);
 * oled->ops->self_consult(oled, "height", NULL, &height);
 *
 * printf("Display: %u x %u pixels\\n", width, height);
 * @endcode
 *
 * @see CFBD_OLEDOperations for operation table definition
 */
typedef CFBD_Bool (*OLED_QueryOperation)(CFBD_OLED* oled,
                                         const char* property,
                                         void* args,
                                         void* request_data);

/**
 * @struct CFBD_OLEDOperations
 * @brief Virtual operation table implementing OLED driver functionality.
 *
 * @details
 * Concrete device drivers populate an instance of this table and assign
 * it to the ops member of a CFBD_OLED object. This structure defines
 * the complete interface for display operations, lifecycle management,
 * and device queries. Callers should only invoke functions through this
 * table after device initialization.
 *
 * The operation table enables:
 * - Device initialization and lifecycle (init, open, close)
 * - Pixel and area rendering (setPixel, setArea)
 * - Frame operations (update, clear, revert)
 * - Rectangular area operations (update_area, clear_area, revert_area)
 * - Device capability queries (self_consult)
 *
 * @note
 * All function pointers must be non-NULL unless explicitly stated otherwise.
 * Device implementations should handle edge cases (out-of-bounds coordinates)
 * gracefully by clipping or returning error codes.
 *
 * @example
 * @code
 * // Example: Using the operation table
 * CFBD_OLED oled_device;
 *
 * // Initialize device
 * oled_device.ops->init(&oled_device, NULL);
 *
 * // Open/enable the display
 * oled_device.ops->open(&oled_device);
 *
 * // Set individual pixels
 * for (uint16_t y = 0; y < 32; y++) {
 *     oled_device.ops->setPixel(&oled_device, y, y);
 * }
 *
 * // Update display
 * oled_device.ops->update(&oled_device);
 *
 * // Close/disable the display
 * oled_device.ops->close(&oled_device);
 * @endcode
 */
typedef struct _CFBD_OLED_OPS
{
    /**
     * @brief Initialize the OLED device.
     *
     * @param oled Pointer to the CFBD_OLED instance.
     * @param init_args Optional device-specific initialization arguments.
     * @return int Status code; typically 0 for success, negative for errors.
     */
    int (*init)(CFBD_OLED* oled, void* init_args);

    /**
     * @brief Set a single pixel at (x, y) to on/enabled state.
     *
     * @param oled Pointer to the CFBD_OLED instance.
     * @param x X coordinate of the pixel.
     * @param y Y coordinate of the pixel.
     * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
     */
    CFBD_Bool (*setPixel)(CFBD_OLED* oled, uint16_t x, uint16_t y);

    /**
     * @brief Write a block of pixel data to a rectangular area.
     *
     * @param device Pointer to the CFBD_OLED instance.
     * @param x X coordinate of the area's top-left corner.
     * @param y Y coordinate of the area's top-left corner.
     * @param width Width of the area in pixels.
     * @param height Height of the area in pixels.
     * @param source Pointer to source bitmap data. Format is device-dependent.
     * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
     */
    CFBD_Bool (*setArea)(CFBD_OLED* device,
                         uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height,
                         uint8_t* source);

    /**
     * @brief Push the local frame buffer to the display.
     *
     * @details
     * Synchronizes the local frame buffer (frame RAM) with the display,
     * making all buffered changes visible on the physical display.
     */
    FrameOperation update;

    /**
     * @brief Clear the entire display and frame buffer.
     *
     * @details
     * Clears all pixels on the display and in the local frame buffer,
     * typically by writing zero bytes to all GRAM locations.
     */
    FrameOperation clear;

    /**
     * @brief Revert the display to the previous frame content (if supported).
     *
     * @details
     * Restores the previously displayed content. Not all devices support
     * this operation; implementation may return CFBD_FALSE if unavailable.
     */
    FrameOperation revert;

    /**
     * @brief Update a rectangular area on the display.
     */
    AreaOperations update_area;

    /**
     * @brief Clear a rectangular area on the display and buffer.
     */
    AreaOperations clear_area;

    /**
     * @brief Revert a rectangular area to its previous content (if supported).
     */
    AreaOperations revert_area;

    /**
     * @brief Open/enable the display device.
     *
     * @details
     * Powers on the display and initializes hardware resources.
     * Must be called before using display operations.
     */
    OLEDSelfOperation open;

    /**
     * @brief Close/disable the display device.
     *
     * @details
     * Powers off the display and releases hardware resources.
     */
    OLEDSelfOperation close;

    /**
     * @brief Query device properties and capabilities.
     *
     * @details
     * Retrieves device-specific information such as resolution, color depth,
     * and supported features. Implementations must support at least:
     * - "rgb" (CFBD_Bool): CFBD_TRUE for RGB, CFBD_FALSE for monochrome
     * - "width" (uint16_t): Display width in pixels
     * - "height" (uint16_t): Display height in pixels
     *
     * Additional device-specific properties may be queried as needed.
     */
    OLED_QueryOperation self_consult;
} CFBD_OLEDOperations;

/**
 * @struct CFBD_OLED
 * @brief The generic OLED device object exposed to application code.
 *
 * @details
 * This is the primary interface through which application code interacts with
 * OLED displays. It encapsulates the operation table, device type, and
 * backend-specific state into a single, transport-agnostic interface.
 *
 * Typical usage pattern:
 * 1. Call CFBD_GetOLEDHandle() to obtain and initialize an instance
 * 2. Call ops->open() to enable the device
 * 3. Use rendering operations (setPixel, setArea, etc.) to draw
 * 4. Call ops->update() to push changes to the display
 * 5. Call ops->close() when done
 *
 * @note
 * The oled_internal_handle is opaque and device-specific. Do not access it directly;
 * all device interaction must occur through the ops table.
 */
typedef struct _CFBD_OLED
{
    /**
     * @brief Pointer to the operation table.
     *
     * @details
     * Device drivers populate this with concrete implementations of all
     * OLED operations. Must be non-NULL after device initialization.
     */
    const CFBD_OLEDOperations* ops;

    /**
     * @brief The underlying transport/driver type used for this device.
     *
     * @details
     * Identifies whether communication occurs via I2C, SPI, or another
     * transport mechanism. Affects the format of initialization parameters
     * and internal implementation details.
     */
    CFBD_OLEDDriverType driver_type;

    /**
     * @brief Opaque backend-specific handle and state.
     *
     * @details
     * Contains driver-specific state, including transport handles,
     * device addresses, frame buffers, and callback contexts.
     * Callers must not access or dereference this directly.
     */
    CFBD_OLEDHandle oled_internal_handle;
} CFBD_OLED;

/**
 * @typedef CFBDOLED_Params_Inits
 * @brief Opaque pointer type for transport-specific initialization parameters.
 *
 * @details
 * This typedef abstracts transport-specific parameter structures.
 * For I2C transport: point to CFBD_OLED_IICInitsParams
 * For SPI transport: point to device-specific SPI parameter structure
 * For other transports: point to relevant parameter structure
 *
 * @see CFBD_GetOLEDHandle() for usage
 */
typedef void* CFBDOLED_Params_Inits;

/**
 * @brief Factory function: acquire or initialize an OLED device handle.
 *
 * @details
 * Creates a new OLED device handle by selecting a device type, binding it to
 * a communication transport, and optionally performing immediate hardware
 * initialization. On success, the provided CFBD_OLED structure is populated
 * with operation table, driver type, and backend-specific state.
 *
 * Transport-specific parameters (args) should be:
 * - For I2C (CFBD_OLEDDriverType_IIC): pointer to CFBD_OLED_IICInitsParams
 * - For SPI (CFBD_OLEDDriverType_SPI): pointer to SPI-specific parameters
 *
 * If request_immediate_init is CFBD_FALSE, the device is configured but not
 * initialized; call ops->init() and ops->open() explicitly.
 *
 * @param oled Pointer to an uninitialized CFBD_OLED structure.
 *             This structure will be populated with device information on success.
 *             The caller must ensure oled remains valid for the device lifetime.
 *
 * @param driver_type Transport/driver type to use (IIC or SPI).
 *                    Determines which backend implementation to load.
 *
 * @param args Transport-specific initialization parameters.
 *             For I2C: CFBD_OLED_IICInitsParams* with device address and I2C handle.
 *             For SPI: SPI-specific configuration structure.
 *             May be NULL for some transports if defaults are acceptable.
 *
 * @param request_immediate_init If CFBD_TRUE, perform hardware initialization
 *                               and open the device immediately (call init and open).
 *                               If CFBD_FALSE, only configure the device; explicit
 *                               init/open calls are required before use.
 *
 * @return CFBD_Bool CFBD_TRUE on success (oled is now valid for use or after
 *                   calling open), CFBD_FALSE on failure (oled is unchanged).
 *                   Check for specific error codes via ops->self_consult()
 *
 * @note
 * - The oled parameter must not be NULL and must remain valid for the device lifetime
 * - args validity depends on driver_type; provide correct parameter types
 * - Even if request_immediate_init is CFBD_TRUE, call ops->open() before using
 *   display operations
 * - Multiple OLED instances can coexist; each gets its own backend handle
 *
 * @example
 * @code
 * // Example 1: Initialize SSD1309 via I2C with immediate init
 * #include "oled.h"
 * #include "driver/backend/oled_iic.h"
 * #include "driver/device/ssd1309/ssd1309.h"
 *
 * CFBD_OLED oled_device;
 * CFBD_OLED_IICInitsParams iic_config = {
 *     .device_address = SSD1309_DRIVER_ADDRESS,
 *     .i2c_interface = my_i2c,
 *     // ... other I2C parameters
 * };
 *
 * if (CFBD_GetOLEDHandle(&oled_device,
 *                        CFBD_OLEDDriverType_IIC,
 *                        &iic_config,
 *                        CFBD_TRUE)) {
 *     // Device is initialized; open and use it
 *     oled_device.ops->open(&oled_device);
 *     oled_device.ops->setPixel(&oled_device, 64, 32);
 *     oled_device.ops->update(&oled_device);
 * }
 * @endcode
 *
 * @example
 * @code
 * // Example 2: Deferred initialization
 * CFBD_OLED oled_device;
 * CFBD_OLED_IICInitsParams iic_config = {...};
 *
 * // Create handle but don't initialize yet
 * if (CFBD_GetOLEDHandle(&oled_device,
 *                        CFBD_OLEDDriverType_IIC,
 *                        &iic_config,
 *                        CFBD_FALSE)) {
 *     // Later, when ready to use the device:
 *     oled_device.ops->init(&oled_device, NULL);
 *     oled_device.ops->open(&oled_device);
 *     // Now device is ready for operations
 * }
 * @endcode
 *
 * @see CFBD_OLED for device structure
 * @see CFBD_OLEDOperations for available operations
 * @see oled_iic.h for I2C initialization details
 * @see ssd1309.h for SSD1309 device-specific information
 */
CFBD_Bool CFBD_GetOLEDHandle(CFBD_OLED* oled,
                             const CFBD_OLEDDriverType driver_type,
                             CFBDOLED_Params_Inits args,
                             CFBD_Bool request_immediate_init);

/** @} */ // end of OLED group
