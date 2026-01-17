/**
 * @file graphic_device.h
 * @brief Generic graphics device abstraction layer for rendering operations.
 * @ingroup Graphics_Device
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @version 0.1
 * @date 2026-01-05
 * @copyright Copyright (c) 2026
 *
 * @details
 * This module provides a unified abstraction for graphics rendering devices
 * (such as OLED displays). It defines a virtual operation table that allows
 * different hardware devices to be controlled through a common interface.
 *
 * The graphics device layer sits above device-specific implementations and
 * provides services for pixel drawing, area operations, frame management,
 * and device property queries.
 *
 * @see @ref Graphics_Device for module details
 * @see @ref OLED for OLED display module
 */

#pragma once
#include "cfbd_define.h"

/**
 * @defgroup Graphics_Device Graphics Device Abstraction
 * @ingroup Graphics
 * @brief Generic graphics device interface and operations.
 *
 * @details
 * This module defines the generic graphics device interface used by the
 * graphics subsystem. It provides a unified API for controlling various
 * graphics display hardware (OLED, LCD, LED matrices, etc.) through a
 * standardized operation table.
 *
 * Key features:
 * - Device-agnostic rendering interface
 * - Support for immediate or deferred drawing modes
 * - Area-based and frame-based operations
 * - Device property queries
 * - Hardware binding flexibility
 *
 * @see oled_graphic_device.h for OLED device binding
 * @{\n */

typedef struct _CFBD_GraphicDevice CFBD_GraphicDevice;

/**
 * @typedef GraphicAreaOperations
 * @brief Function pointer type for area-based graphics operations.
 *
 * @details
 * This function pointer signature handles rectangular area operations on
 * the graphics device, such as updating, clearing, or reverting specific
 * regions of the display.
 *
 * @param device Pointer to the CFBD_GraphicDevice instance.
 * @param x X coordinate of the area's top-left corner.
 * @param y Y coordinate of the area's top-left corner.
 * @param width Width of the area in pixels.
 * @param height Height of the area in pixels.
 *
 * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
 *
 * @see CFBD_GraphicDeviceOperation for operation table definition
 * @example
 * @code
 * // Example: Clear a rectangular area
 * CFBD_GraphicDevice* device = &my_graphics_device;
 * CFBD_bool result = device->ops->clear_area(device, 10, 10, 50, 30);
 * if (result && device->immediate_draw) {
 *     device->ops->update_area(device, 10, 10, 50, 30);
 * }
 * @endcode
 */
typedef CFBD_Bool (*GraphicAreaOperations)(CFBD_GraphicDevice* device,
                                           uint16_t x,
                                           uint16_t y,
                                           uint16_t width,
                                           uint16_t height);
/**
 * @typedef GraphicFrameOperation
 * @brief Function pointer type for full-frame graphics operations.
 *
 * @details
 * This function pointer signature handles operations that affect the entire
 * graphics frame, such as updating the complete display, clearing all pixels,
 * or reverting to the previous frame content.
 *
 * @param device Pointer to the CFBD_GraphicDevice instance.
 * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
 *
 * @see CFBD_GraphicDeviceOperation for operation table definition
 */
typedef CFBD_Bool (*GraphicFrameOperation)(CFBD_GraphicDevice* device);
/**
 * @typedef GraphicOLEDSelfOperation
 * @brief Function pointer type for device lifecycle operations.
 *
 * @details
 * This function pointer signature handles device lifecycle management
 * operations such as opening (enabling) or closing (disabling) the
 * graphics device.
 *
 * @param device Pointer to the CFBD_GraphicDevice instance.
 * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
 *
 * @see CFBD_GraphicDeviceOperation for operation table definition
 */
typedef CFBD_Bool (*GraphicOLEDSelfOperation)(CFBD_GraphicDevice* device);

/**
 * @typedef GraphicOLED_QueryOperation
 * @brief Function pointer type for querying device properties.
 *
 * @details
 * This function pointer signature handles runtime queries for device
 * capabilities and properties. Implementations should support standard
 * properties: "width" (uint16_t), "height" (uint16_t), and "rgb" (CFBD_Bool).
 *
 * @param device Pointer to the CFBD_GraphicDevice instance.
 * @param property Null-terminated string naming the queried property.
 * @param args Optional input arguments for the query (implementation-defined).
 * @param request_data Output pointer where query results are written.
 *
 * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
 *
 * @example
 * @code
 * // Example: Query device resolution
 * CFBD_GraphicDevice* device = &my_graphics_device;
 * uint16_t width = 0, height = 0;
 *
 * device->ops->self_consult(device, "width", NULL, &width);
 * device->ops->self_consult(device, "height", NULL, &height);
 * printf("Device Resolution: %u x %u\n", width, height);
 * @endcode
 *
 * @see CFBD_GraphicDeviceOperation for operation table definition
 */
typedef CFBD_Bool (*GraphicOLED_QueryOperation)(CFBD_GraphicDevice* device,
                                                const char* property,
                                                void* args,
                                                void* request_data);

/**
 * @typedef GraphicOLED_PropertySetsOperation
 * @brief Function pointer type for settings device properties.
 *
 * @details
 * This function pointer signature handles runtime sets for device
 * capabilities and properties. Implementations should support standard.
 *
 * @param device Pointer to the CFBD_GraphicDevice instance.
 * @param property Null-terminated string naming the queried property.
 * @param args Optional input arguments for the query (implementation-defined).
 * @param request_data Output pointer where to write results are.
 *
 * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
 *
 *
 * @see CFBD_GraphicDeviceOperation for operation table definition
 */
typedef CFBD_Bool (*GraphicOLED_PropertySetsOperation)(CFBD_GraphicDevice* device,
                                                       const char* property,
                                                       void* args,
                                                       void* request_data);

/**
 * @struct CFBD_GraphicDeviceOperation
 * @brief Virtual operation table for graphics device functionality.
 *
 * @details
 * This structure defines the complete interface for graphics rendering
 * operations. Device implementations populate this table with concrete
 * implementations tailored to specific hardware capabilities.
 *
 * The operation table provides:
 * - Device initialization and lifecycle management
 * - Pixel and area rendering operations
 * - Frame-level update, clear, and revert operations
 * - Device property queries
 *
 * @note All function pointers must be non-NULL unless explicitly documented otherwise.
 *
 * @example
 * @code
 * // Example: Complete graphics device workflow
 * CFBD_GraphicDevice* device = &my_graphics_device;
 *
 * // Initialize device
 * device->ops->init(device, NULL);
 * device->ops->open(device);
 *
 * // Clear and update display
 * device->ops->clear(device);
 * device->ops->update(device);
 *
 * // Draw a pixel and update if immediate mode
 * device->ops->setPixel(device, 64, 32);
 * if (device->immediate_draw) {
 *     device->ops->update(device);
 * }
 *
 * // Close device
 * device->ops->close(device);
 * @endcode
 */
typedef struct
{
    /**
     * @brief Initialize the graphics device.
     *
     * @param device Pointer to the CFBD_GraphicDevice instance.
     * @param init_args Optional device-specific initialization arguments.
     * @return int Status code; typically 0 for success, negative for errors.
     */
    int (*init)(CFBD_GraphicDevice* device, void* init_args);

    /**
     * @brief Set a single pixel at (x, y) to the active drawing color.
     *
     * @param device Pointer to the CFBD_GraphicDevice instance.
     * @param x X coordinate of the pixel.
     * @param y Y coordinate of the pixel.
     * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
     */
    CFBD_Bool (*setPixel)(CFBD_GraphicDevice* device, uint16_t x, uint16_t y);

    /**
     * @brief Write a block of pixel data to a rectangular area.
     *
     * @param device Pointer to the CFBD_GraphicDevice instance.
     * @param x X coordinate of the area's top-left corner.
     * @param y Y coordinate of the area's top-left corner.
     * @param width Width of the area in pixels.
     * @param height Height of the area in pixels.
     * @param source Pointer to source pixel data. Format is device-dependent.
     * @return CFBD_Bool CFBD_TRUE on success, CFBD_FALSE on failure.
     */
    CFBD_Bool (*setArea)(CFBD_GraphicDevice* device,
                         uint16_t x,
                         uint16_t y,
                         uint16_t width,
                         uint16_t height,
                         uint8_t* source);

    /**
     * @brief Push the local frame buffer to the display.
     *
     * @details
     * Synchronizes the local frame buffer with the physical display,
     * making all buffered changes visible.
     */
    GraphicFrameOperation update;

    /**
     * @brief Clear the entire display and frame buffer.
     *
     * @details
     * Clears all pixels by writing zero/off states to all display locations.
     */
    GraphicFrameOperation clear;

    /**
     * @brief Revert to the previous frame content (if supported).
     *
     * @details
     * Not all devices support frame revert; may return CFBD_FALSE if unavailable.
     */
    GraphicFrameOperation revert;

    /**
     * @brief Update a rectangular area on the display.
     */
    GraphicAreaOperations update_area;

    /**
     * @brief Clear a rectangular area on the display and buffer.
     */
    GraphicAreaOperations clear_area;

    /**
     * @brief Revert a rectangular area to its previous content.
     */
    GraphicAreaOperations revert_area;

    /**
     * @brief Open/enable the graphics device.
     *
     * @details
     * Powers on the device and prepares it for rendering operations.
     */
    GraphicOLEDSelfOperation open;

    /**
     * @brief Close/disable the graphics device.
     *
     * @details
     * Powers off the device and releases hardware resources.
     */
    GraphicOLEDSelfOperation close;

    /**
     * @brief Query device properties and capabilities.
     *
     * @details
     * Supported standard properties:
     * - "width" (uint16_t): Display width in pixels
     * - "height" (uint16_t): Display height in pixels
     * - "rgb" (CFBD_Bool): RGB color support indicator
     */
    GraphicOLED_QueryOperation self_consult;

    /**
     * @brief Set the properties for devices supports
     *  - "color": if the devices supports color, thats it :)
     */
    GraphicOLED_PropertySetsOperation self_sets;
} CFBD_GraphicDeviceOperation;

/**
 * @enum CFBDGraphic_DeviceType
 * @brief Enumeration of supported graphics device types.
 *
 * @details
 * Specifies which type of graphics hardware device is being used.
 * Each device type may have different initialization parameters and
 * rendering characteristics.
 */
typedef enum
{
    OLED /**< OLED display device */
} CFBDGraphic_DeviceType;

/**
 * @typedef CFBDGraphicDeviceHandle
 * @brief Opaque handle type for device-specific state.
 */
typedef void* CFBDGraphicDeviceHandle;

/**
 * @struct CFBD_GraphicDevice
 * @brief The main graphics device object.
 *
 * @details
 * This structure represents a graphics rendering device and encapsulates
 * the operation table, device type, internal state, and drawing mode
 * settings. Application code interacts with devices through this structure.
 *
 * Typical usage flow:
 * 1. Initialize device via CFBDGraphic_BindDevice()
 * 2. Open device via ops->open()
 * 3. Perform drawing operations
 * 4. Update display via ops->update()
 * 5. Close device via ops->close()
 *
 * @note The internal_handle is opaque and device-specific;
 *       do not access it directly.
 */
typedef struct _CFBD_GraphicDevice
{
    /**
     * @brief Pointer to the operation table.
     *
     * @details
     * Device implementations populate this with concrete operations.
     * Must be non-NULL after device initialization.
     */
    CFBD_GraphicDeviceOperation* ops;

    /**
     * @brief The graphics device type.
     *
     * @details
     * Identifies the hardware type (OLED, LCD, etc.).
     * Determines device-specific behavior and capabilities.
     */
    CFBDGraphic_DeviceType device_type;

    /**
     * @brief Opaque device-specific handle and state.
     *
     * @details
     * Contains implementation-specific data such as hardware handles,
     * configuration parameters, and internal buffers.
     * Must not be accessed or modified directly by application code.
     */
    CFBDGraphicDeviceHandle internal_handle;

    /**
     * @brief Immediate draw mode flag.
     *
     * @details
     * When CFBD_TRUE, each drawing operation automatically updates
     * the display immediately. When CFBD_FALSE, drawing operations
     * modify the frame buffer but do not update the display until
     * ops->update() is explicitly called (batch mode).
     *
     * This enables flexible control over rendering performance and
     * visual updates.
     */
    CFBD_Bool immediate_draw;
} CFBD_GraphicDevice;

/**
 * @brief Check if the device is in immediate draw mode.
 *
 * @details
 * In immediate mode, each drawing operation automatically updates
 * the display. In deferred mode, updates must be requested explicitly.
 *
 * @param device Pointer to the CFBD_GraphicDevice instance.
 * @return CFBD_Bool CFBD_TRUE if in immediate draw mode, CFBD_FALSE
 * otherwise.
 *
 * @see CFBDGraphic_DeviceSetIfRequestUpdateAtOnce() to change the mode
 * @example
 * @code
 * if (CFBDGraphic_DeviceRequestUpdateAtOnce(&device)) {
 *     // Device is in immediate draw mode
 * }
 * @endcode
 */
static inline CFBD_Bool CFBDGraphic_DeviceRequestUpdateAtOnce(CFBD_GraphicDevice* device)
{
    return device->immediate_draw;
}

/**
 * @brief Set the immediate draw mode for the graphics device.
 *
 * @details
 * Configure whether the device should automatically update the display
 * after each drawing operation (immediate mode) or accumulate changes
 * and require explicit update calls (deferred mode).
 *
 * Immediate mode is useful for real-time interactive applications.
 * Deferred mode allows batch operations for better performance.
 *
 * @param device Pointer to the CFBD_GraphicDevice instance.
 * @param requests CFBD_TRUE to enable immediate draw mode,
 *                 CFBD_FALSE to disable (deferred mode).
 *
 * @see CFBDGraphic_DeviceRequestUpdateAtOnce() to check current mode
 * @example
 * @code
 * // Enable immediate draw mode
 * CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(&device, CFBD_TRUE);
 *
 * // Now each drawing operation updates the display immediately
 * device.ops->setPixel(&device, 64, 32);  // Visible on display
 * @endcode
 */
static inline void CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(CFBD_GraphicDevice* device,
                                                              CFBD_Bool requests)
{
    device->immediate_draw = requests;
}

/**
 * @brief Convenience function to clear and immediately update the display.
 *
 * @details
 * This is a utility function that clears all pixels and synchronizes
 * the changes to the physical display in one operation. Equivalent to:
 * @code
 * device->ops->clear(device);
 * device->ops->update(device);
 * @endcode
 *
 * @param device Pointer to the CFBD_GraphicDevice instance.
 *
 * @see CFBD_GraphicDevice for device structure
 * @example
 * @code
 * // Clear the display completely
 * CFBDGraphic_DeviceClearImmediate(&graphics_device);
 * @endcode
 */
static inline void CFBDGraphic_DeviceClearImmediate(CFBD_GraphicDevice* device)
{
    device->ops->clear(device);
    device->ops->update(device);
}

/**
 * @brief Bind a graphics device to a physical hardware device.
 *
 * @details
 * Associates a graphics device object with concrete hardware implementation.
 * This function initializes the device structure with the appropriate
 * operation table and internal handle based on the device type.
 *
 * After binding, the device must be opened via ops->open() before use.
 *
 * @param device Pointer to the CFBD_GraphicDevice to initialize.
 * @param device_type The type of graphics hardware (e.g., OLED).
 * @param internal_handle Device-specific handle or state pointer.
 *                        For OLED devices: pass pointer to CFBD_OLED*.
 *
 * @note The internal_handle must remain valid for the device lifetime.
 *
 * @see CFBDGraphic_BindOLEDAsDevice() for OLED-specific binding
 * @example
 * @code
 * // Bind an OLED display as graphics device
 * CFBD_OLED oled_display;
 * CFBD_GraphicDevice graphics;
 *
 * // Initialize OLED first
 * CFBD_GetOLEDHandle(&oled_display, CFBD_OLEDDriverType_IIC, &iic_params, CFBD_TRUE);
 *
 * // Bind as graphics device
 * CFBDGraphic_BindDevice(&graphics, OLED, &oled_display);
 * graphics.ops->open(&graphics);
 * @endcode
 */
void CFBDGraphic_BindDevice(CFBD_GraphicDevice* device,
                            CFBDGraphic_DeviceType device_type,
                            CFBDGraphicDeviceHandle internal_handle);

/** @} */ // end of Graphics_Device group