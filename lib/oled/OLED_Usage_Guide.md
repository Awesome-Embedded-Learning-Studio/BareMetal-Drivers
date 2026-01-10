/**
 * @page OLED_UsageGuide OLED Driver Usage Guide
 *
 * @tableofcontents
 *
 * @section Overview
 *
 * The OLED subsystem provides a generic, transport-agnostic interface for
 * controlling OLED displays. It supports multiple device types (SSD1306, SSD1309, etc.)
 * and multiple communication backends (I2C, SPI).
 *
 * @section Architecture
 *
 * The OLED driver architecture consists of three main layers:
 *
 * - **Generic OLED Driver**: The top-level interface (`oled.h`) that application
 *   code interacts with. Provides a device-agnostic API for rendering and control.
 *
 * - **Device Drivers**: Device-specific implementations (SSD1306, SSD1309) that
 *   provide device-specific initialization, memory layouts, and communication prefixes.
 *
 * - **Transport Backends**: Transport-specific implementations (I2C, SPI) that
 *   handle the actual hardware communication with the device.
 *
 * @section GettingStarted
 *
 * @subsection Step1_HeaderFiles Include Required Headers
 *
 * ```cpp
 * #include "oled.h"
 * #include "driver/backend/oled_iic.h"
 * #include "driver/device/ssd1309/ssd1309.h"
 * ```
 *
 * @subsection Step2_InitializeI2C Initialize I2C Transport
 *
 * Before initializing the OLED device, ensure your I2C interface is properly
 * configured. Prepare the I2C initialization parameters:
 *
 * ```cpp
 * CFBD_OLED_IICInitsParams i2c_config = {
 *     .device_address = SSD1309_DRIVER_ADDRESS,  // 0x78
 *     .i2c_handle = &my_i2c_interface,
 *     .notify_tx_complete = on_i2c_transmit_complete,
 *     .notify_error = on_i2c_error,
 * };
 * ```
 *
 * @subsection Step3_ObtainOLEDHandle Get OLED Device Handle
 *
 * Use `CFBD_GetOLEDHandle()` to create and initialize an OLED device:
 *
 * ```cpp
 * CFBD_OLED oled_device;
 *
 * if (CFBD_GetOLEDHandle(&oled_device,
 *                        CFBD_OLEDDriverType_IIC,
 *                        &i2c_config,
 *                        CFBD_TRUE)) {  // Request immediate initialization
 *     // Device handle created successfully
 * }
 * ```
 *
 * @subsection Step4_EnableDisplay Enable the Display
 *
 * Open/enable the OLED display:
 *
 * ```cpp
 * if (oled_device.ops->open(&oled_device)) {
 *     // Display is now powered on and ready
 * }
 * ```
 *
 * @section CommonOperations
 *
 * @subsection ClearDisplay Clear the Entire Display
 *
 * ```cpp
 * // Clear all pixels
 * oled_device.ops->clear(&oled_device);
 *
 * // Push changes to physical display
 * oled_device.ops->update(&oled_device);
 * ```
 *
 * @subsection DrawPixel Draw Individual Pixels
 *
 * ```cpp
 * // Set pixel at (x=64, y=32) to on
 * CFBD_bool success = oled_device.ops->setPixel(&oled_device, 64, 32);
 *
 * // Update display to show the pixel
 * oled_device.ops->update(&oled_device);
 * ```
 *
 * @subsection DrawArea Draw a Block of Data
 *
 * ```cpp
 * // Example: Draw a 16x16 bitmap at position (0, 0)
 * uint8_t bitmap_data[32];  // 16x16 pixels, monochrome format
 *
 * oled_device.ops->setArea(&oled_device,
 *                          0, 0,           // x, y position
 *                          16, 16,         // width, height
 *                          bitmap_data);   // source data
 *
 * // Update the area on display
 * oled_device.ops->update_area(&oled_device, 0, 0, 16, 16);
 * ```
 *
 * @subsection QueryDeviceProperties Query Display Properties
 *
 * ```cpp
 * // Query display width
 * uint16_t width = 0;
 * oled_device.ops->self_consult(&oled_device, "width", NULL, &width);
 *
 * // Query display height
 * uint16_t height = 0;
 * oled_device.ops->self_consult(&oled_device, "height", NULL, &height);
 *
 * // Query color support (RGB vs monochrome)
 * CFBD_bool is_rgb = CFBD_FALSE;
 * oled_device.ops->self_consult(&oled_device, "rgb", NULL, &is_rgb);
 *
 * printf("Display: %u x %u, Color Mode: %s\n",
 *        width, height, is_rgb ? "RGB" : "Monochrome");
 * ```
 *
 * @subsection CloseDisplay Close/Disable the Display
 *
 * When finished with the display, close it to power off and release resources:
 *
 * ```cpp
 * oled_device.ops->close(&oled_device);
 * ```
 *
 * @section CompleteExample
 *
 * Here's a complete example demonstrating the OLED driver usage:
 *
 * ```cpp
 * #include "oled.h"
 * #include "driver/backend/oled_iic.h"
 * #include "driver/device/ssd1309/ssd1309.h"
 * #include <stdio.h>
 *
 * void oled_demo(void) {
 *     // Step 1: Setup I2C configuration
 *     CFBD_OLED_IICInitsParams i2c_config = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,
 *         .i2c_handle = get_i2c_interface(),
 *     };
 *
 *     // Step 2: Get OLED device handle
 *     CFBD_OLED oled_device;
 *     if (!CFBD_GetOLEDHandle(&oled_device,
 *                             CFBD_OLEDDriverType_IIC,
 *                             &i2c_config,
 *                             CFBD_TRUE)) {
 *         printf("Failed to initialize OLED device\n");
 *         return;
 *     }
 *
 *     // Step 3: Open the display
 *     if (!oled_device.ops->open(&oled_device)) {
 *         printf("Failed to open OLED device\n");
 *         return;
 *     }
 *
 *     // Step 4: Query and display device information
 *     uint16_t width = 0, height = 0;
 *     oled_device.ops->self_consult(&oled_device, "width", NULL, &width);
 *     oled_device.ops->self_consult(&oled_device, "height", NULL, &height);
 *     printf("OLED Display: %u x %u pixels\n", width, height);
 *
 *     // Step 5: Clear the display
 *     oled_device.ops->clear(&oled_device);
 *     oled_device.ops->update(&oled_device);
 *
 *     // Step 6: Draw some pixels
 *     for (uint16_t x = 0; x < width; x += 2) {
 *         oled_device.ops->setPixel(&oled_device, x, 10);
 *     }
 *     oled_device.ops->update(&oled_device);
 *
 *     // Step 7: Close the display
 *     oled_device.ops->close(&oled_device);
 * }
 * ```
 *
 * @section SupportedDevices
 *
 * Currently supported OLED controller chips:
 * - **SSD1306**: Common monochrome controller, 128x64 resolution typical
 * - **SSD1309**: Enhanced version with improved contrast and timing control
 *
 * @section SupportedTransports
 *
 * Available transport backends:
 * - **I2C (IIC)**: Two-wire interface, default at 0x78 for SSD1309
 * - **SPI**: High-speed serial interface (implementation available)
 *
 * @section ErrorHandling
 *
 * Most operations return `CFBD_bool` (TRUE/FALSE) to indicate success or failure.
 * Common error conditions:
 * - Invalid device address
 * - I2C communication errors
 * - Device not initialized or opened
 * - Out-of-bounds coordinate access
 *
 * Always check return values before proceeding with display operations.
 *
 * @section Performance Considerations
 *
 * - **Full frame updates**: Use `update()` for complete display refresh
 * - **Partial updates**: Use `update_area()` for better performance when
 *   updating only specific regions
 * - **Buffering**: The driver maintains a local frame buffer; changes are
 *   not visible on the display until `update()` or `update_area()` is called
 *
 * @section Troubleshooting
 *
 * **Display not responding**: Verify I2C address and electrical connections
 *
 * **Garbled display**: Check frame buffer data format and pixel ordering
 *
 * **Slow updates**: Consider using area-based updates instead of full updates
 *
 * @section References
 *
 * - @ref OLED Main OLED driver module
 * - @ref OLED_Device Device interface documentation
 * - @ref OLED_Backend Backend transport implementations
 *
 */
