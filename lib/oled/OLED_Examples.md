/**
 * @page OLED_Examples OLED Driver Examples
 *
 * @tableofcontents
 *
 * This page contains comprehensive code examples demonstrating how to use
 * the OLED driver for various common tasks.
 *
 * @section Example1_BasicInitialization Basic Device Initialization
 *
 * The most fundamental example showing device setup and initialization:
 *
 * @code
 * #include "oled.h"
 * #include "driver/backend/oled_iic.h"
 * #include "driver/device/ssd1309/ssd1309.h"
 *
 * void example_basic_init(void) {
 *     // Create OLED device structure
 *     CFBD_OLED oled;
 *
 *     // Setup I2C parameters
 *     CFBD_OLED_IICInitsParams i2c_params = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,  // Use standard address
 *         .i2c_handle = get_my_i2c_interface(),
 *     };
 *
 *     // Initialize OLED with I2C backend
 *     CFBD_bool success = CFBD_GetOLEDHandle(
 *         &oled,
 *         CFBD_OLEDDriverType_IIC,
 *         &i2c_params,
 *         CFBD_TRUE  // Immediate initialization
 *     );
 *
 *     if (success && oled.ops->open(&oled)) {
 *         // Device is now ready for use
 *     }
 * }
 * @endcode
 *
 * @section Example2_ClearAndDisplay Clear Display
 *
 * Clear all pixels and update the physical display:
 *
 * @code
 * void example_clear_display(CFBD_OLED* oled) {
 *     // Clear all pixels in the local buffer
 *     oled->ops->clear(oled);
 *
 *     // Synchronize with physical display
 *     oled->ops->update(oled);
 * }
 * @endcode
 *
 * @section Example3_DrawingPixels Drawing Individual Pixels
 *
 * Set individual pixels to create a pattern:
 *
 * @code
 * void example_draw_checkerboard(CFBD_OLED* oled) {
 *     // Get display dimensions
 *     uint16_t width = 0, height = 0;
 *     oled->ops->self_consult(oled, "width", NULL, &width);
 *     oled->ops->self_consult(oled, "height", NULL, &height);
 *
 *     // Draw checkerboard pattern
 *     for (uint16_t y = 0; y < height; y += 2) {
 *         for (uint16_t x = 0; x < width; x += 2) {
 *             if ((x + y) % 4 == 0) {
 *                 oled->ops->setPixel(oled, x, y);
 *             }
 *         }
 *     }
 *
 *     // Update display
 *     oled->ops->update(oled);
 * }
 * @endcode
 *
 * @section Example4_AreaOperations Area-Based Drawing
 *
 * Efficiently update rectangular regions of the display:
 *
 * @code
 * void example_draw_rectangle(CFBD_OLED* oled,
 *                             uint16_t x, uint16_t y,
 *                             uint16_t width, uint16_t height) {
 *     // Prepare bitmap data (format depends on device)
 *     uint8_t* bitmap = prepare_rectangle_data(width, height);
 *
 *     // Write the area
 *     CFBD_bool success = oled->ops->setArea(oled, x, y, width, height, bitmap);
 *
 *     if (success) {
 *         // Update only this area on the display
 *         oled->ops->update_area(oled, x, y, width, height);
 *     }
 *
 *     free_bitmap_data(bitmap);
 * }
 * @endcode
 *
 * @section Example5_DeviceQueries Device Property Queries
 *
 * Query device capabilities and properties:
 *
 * @code
 * void example_query_device(CFBD_OLED* oled) {
 *     uint16_t width = 0, height = 0;
 *     CFBD_bool is_rgb = CFBD_FALSE;
 *
 *     // Query basic properties
 *     oled->ops->self_consult(oled, "width", NULL, &width);
 *     oled->ops->self_consult(oled, "height", NULL, &height);
 *     oled->ops->self_consult(oled, "rgb", NULL, &is_rgb);
 *
 *     // Report display information
 *     printf("Display Resolution: %u x %u pixels\n", width, height);
 *     printf("Color Mode: %s\n", is_rgb ? "RGB Color" : "Monochrome");
 *     printf("Total Pixels: %u\n", (unsigned long)width * height);
 * }
 * @endcode
 *
 * @section Example6_AreaClear Clearing Specific Regions
 *
 * Clear only a portion of the display:
 *
 * @code
 * void example_clear_area(CFBD_OLED* oled,
 *                        uint16_t x, uint16_t y,
 *                        uint16_t width, uint16_t height) {
 *     // Clear rectangular area in the frame buffer
 *     CFBD_bool success = oled->ops->clear_area(oled, x, y, width, height);
 *
 *     if (success) {
 *         // Update the cleared area on the physical display
 *         oled->ops->update_area(oled, x, y, width, height);
 *     }
 * }
 * @endcode
 *
 * @section Example7_MultipleDevices Multiple OLED Devices
 *
 * Control multiple OLED displays on different I2C addresses:
 *
 * @code
 * #define NUM_DISPLAYS 2
 *
 * void example_multiple_displays(void) {
 *     CFBD_OLED oled_displays[NUM_DISPLAYS];
 *
 *     // Configuration for each display
 *     struct {
 *         uint8_t address;
 *     } configs[NUM_DISPLAYS] = {
 *         { 0x78 },  // First device
 *         { 0x7A },  // Second device (alternate address)
 *     };
 *
 *     // Initialize all displays
 *     for (int i = 0; i < NUM_DISPLAYS; i++) {
 *         CFBD_OLED_IICInitsParams i2c_params = {
 *             .device_address = configs[i].address,
 *             .i2c_handle = get_i2c_interface(),
 *         };
 *
 *         if (CFBD_GetOLEDHandle(&oled_displays[i],
 *                                CFBD_OLEDDriverType_IIC,
 *                                &i2c_params,
 *                                CFBD_TRUE)) {
 *             oled_displays[i].ops->open(&oled_displays[i]);
 *         }
 *     }
 *
 *     // Use both displays independently
 *     for (int i = 0; i < NUM_DISPLAYS; i++) {
 *         oled_displays[i].ops->clear(&oled_displays[i]);
 *         oled_displays[i].ops->update(&oled_displays[i]);
 *     }
 * }
 * @endcode
 *
 * @section Example8_DeferredInitialization Deferred Initialization
 *
 * Create device handle but delay initialization:
 *
 * @code
 * void example_deferred_init(void) {
 *     CFBD_OLED oled;
 *
 *     CFBD_OLED_IICInitsParams i2c_params = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,
 *         .i2c_handle = get_i2c_interface(),
 *     };
 *
 *     // Create handle without immediate initialization
 *     if (CFBD_GetOLEDHandle(&oled,
 *                            CFBD_OLEDDriverType_IIC,
 *                            &i2c_params,
 *                            CFBD_FALSE)) {  // Deferred init
 *
 *         // ... perform other setup operations ...
 *
 *         // Later, initialize when needed
 *         oled.ops->init(&oled, NULL);
 *         oled.ops->open(&oled);
 *
 *         // Now ready for use
 *     }
 * }
 * @endcode
 *
 * @section Example9_ErrorHandling Error Handling
 *
 * Proper error checking throughout device usage:
 *
 * @code
 * CFBD_bool safe_display_operation(void) {
 *     CFBD_OLED oled;
 *
 *     // Step 1: Initialize with error checking
 *     CFBD_OLED_IICInitsParams i2c_params = {
 *         .device_address = SSD1309_DRIVER_ADDRESS,
 *         .i2c_handle = get_i2c_interface(),
 *     };
 *
 *     if (!CFBD_GetOLEDHandle(&oled,
 *                             CFBD_OLEDDriverType_IIC,
 *                             &i2c_params,
 *                             CFBD_TRUE)) {
 *         printf("ERROR: Failed to get OLED handle\n");
 *         return CFBD_FALSE;
 *     }
 *
 *     // Step 2: Open device with error checking
 *     if (!oled.ops->open(&oled)) {
 *         printf("ERROR: Failed to open OLED device\n");
 *         return CFBD_FALSE;
 *     }
 *
 *     // Step 3: Perform operations with error checking
 *     if (!oled.ops->clear(&oled)) {
 *         printf("ERROR: Failed to clear display\n");
 *         oled.ops->close(&oled);
 *         return CFBD_FALSE;
 *     }
 *
 *     if (!oled.ops->update(&oled)) {
 *         printf("ERROR: Failed to update display\n");
 *         oled.ops->close(&oled);
 *         return CFBD_FALSE;
 *     }
 *
 *     // Step 4: Cleanup
 *     oled.ops->close(&oled);
 *     return CFBD_TRUE;
 * }
 * @endcode
 *
 * @section Example10_DisplayText Text Display Pattern
 *
 * Example showing how to structure text display operations:
 *
 * @code
 * void example_display_pattern(CFBD_OLED* oled) {
 *     // Get display dimensions
 *     uint16_t width = 128, height = 64;  // Typical SSD1309
 *
 *     // Clear and prepare
 *     oled->ops->clear(oled);
 *     oled->ops->update(oled);
 *
 *     // Create a pattern (horizontal lines)
 *     for (uint16_t y = 0; y < height; y += 4) {
 *         for (uint16_t x = 0; x < width; x++) {
 *             oled->ops->setPixel(oled, x, y);
 *         }
 *     }
 *
 *     // Update display with new pattern
 *     oled->ops->update(oled);
 *
 *     // Alternative: update only affected area for efficiency
 *     // oled->ops->update_area(oled, 0, 0, width, height);
 * }
 * @endcode
 *
 * @see @ref OLED_UsageGuide for detailed usage documentation
 * @see CFBD_OLED for device structure documentation
 * @see CFBD_OLEDOperations for operation table documentation
 */
