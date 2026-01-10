/**
 * @file oled_graphic_device.h
 * @brief OLED device binding for the graphics subsystem.
 * @ingroup Graphics_Device
 *
 * @details
 * This module provides the integration layer between the generic OLED driver
 * and the graphics device abstraction. It allows OLED displays to be used
 * as rendering targets within the graphics framework.
 *
 * The OLED graphics device adapter translates graphics operations (pixel drawing,
 * area updates, etc.) into OLED-specific commands and memory layouts.
 *
 * @see graphic_device.h for the generic graphics interface
 * @see oled.h for the OLED driver
 *
 * @example
 * @code
 * // Example: Create and use an OLED graphics device
 * #include "oled_graphic_device.h"
 * #include "oled.h"
 *
 * CFBD_OLED oled;
 * CFBD_GraphicDevice graphics;
 *
 * // Initialize OLED display
 * CFBD_OLED_IICInitsParams iic_config = {...};
 * CFBD_GetOLEDHandle(&oled, CFBD_OLEDDriverType_IIC, &iic_config, CFBD_TRUE);
 *
 * // Bind as graphics device
 * CFBDGraphic_BindOLEDAsDevice(&graphics, &oled);
 * graphics.ops->open(&graphics);
 *
 * // Use graphics operations
 * graphics.ops->clear(&graphics);
 * graphics.ops->setPixel(&graphics, 64, 32);
 * graphics.ops->update(&graphics);
 * @endcode
 */

#pragma once
#include "device/graphic_device.h"
#include "oled.h"


/**
 * @defgroup Graphics_OLED OLED Graphics Device Adapter
 * @ingroup Graphics_Device
 * @brief OLED-specific graphics device implementation.
 *
 * @details
 * This module implements the graphics device interface for OLED displays.
 * It bridges the generic graphics API with OLED-specific hardware capabilities
 * and rendering characteristics.
 *
 * The OLED graphics adapter enables:
 * - Standard graphics operations on OLED displays
 * - Seamless integration with the graphics widget framework
 * - Consistent rendering interface across different OLED controllers
 *
 * @{\n */
/**
 * @brief Bind an OLED display to a graphics device.
 *
 * @details
 * Establishes the connection between a graphics device object and an OLED
 * display driver. After binding, the graphics device can be used to render
 * content on the OLED display using standard graphics operations.
 *
 * The graphics device will forward all rendering operations to the underlying
 * OLED device, translating graphics coordinates and pixel data into OLED
 * memory format as needed.
 *
 * @param device Pointer to an uninitialized CFBD_GraphicDevice structure.
 *               This structure will be populated with OLED-specific operations.
 * @param oled Pointer to an initialized CFBD_OLED display device.
 *             The OLED device must already be opened before graphics operations.
 *
 * @note
 * - The OLED device must be initialized and opened before binding
 * - The graphics device takes ownership of the OLED device reference
 * - Multiple graphics devices can share the same OLED device
 *
 * @see CFBDGraphic_BindDevice() for generic device binding
 * @see CFBD_OLED for OLED device structure
 *
 * @example
 * @code
 * // Complete OLED graphics workflow
 * #include "oled_graphic_device.h"
 * #include "oled.h"
 * #include "driver/backend/oled_iic.h"
 *
 * int main(void) {
 *     // Step 1: Initialize OLED display
 *     CFBD_OLED oled;
 *     CFBD_OLED_IICInitsParams iic_params = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,
 *         .i2c_handle = get_i2c_interface(),
 *     };
 *     CFBD_GetOLEDHandle(&oled, CFBD_OLEDDriverType_IIC, &iic_params, CFBD_TRUE);
 *     oled.ops->open(&oled);
 *
 *     // Step 2: Create graphics device and bind OLED
 *     CFBD_GraphicDevice graphics;
 *     CFBDGraphic_BindOLEDAsDevice(&graphics, &oled);
 *     graphics.ops->open(&graphics);
 *
 *     // Step 3: Enable immediate mode for real-time updates
 *     CFBDGraphic_DeviceSetIfRequestUpdateAtOnce(&graphics, CFBD_TRUE);
 *
 *     // Step 4: Render graphics
 *     graphics.ops->clear(&graphics);
 *     graphics.ops->setPixel(&graphics, 64, 32);
 *     graphics.ops->update(&graphics);
 *
 *     // Step 5: Cleanup
 *     graphics.ops->close(&graphics);
 *     oled.ops->close(&oled);
 *     return 0;
 * }
 * @endcode
 */
void CFBDGraphic_BindOLEDAsDevice(CFBD_GraphicDevice* device, CFBD_OLED* oled);

/** @} */ // end of Graphics_OLED group
