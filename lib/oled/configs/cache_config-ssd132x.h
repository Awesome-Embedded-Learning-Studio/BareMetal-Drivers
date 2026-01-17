#pragma once

/**
 * @file cache_config.h
 * @brief Configuration constants for the OLED display pixel cache.
 *
 * @details
 * These macros define the dimensions of the off-screen pixel cache
 * used by the OLED driver. They represent the physical pixel
 * arrangement used when composing frames before sending them to the
 * display device. Adjusting these values requires ensuring the driver
 * and any code that manipulates the cache are updated accordingly.
 *
 * @page oled_cache_page OLED Display Cache Architecture
 * @brief Design and Management of OLED Display Framebuffer
 *
 * @section oled_cache_intro Introduction
 * The OLED cache (framebuffer) is a critical component of the display system.
 * It maintains an off-screen representation of the display content, allowing
 * efficient updates and animations while minimizing I2C traffic.
 *
 * @section oled_cache_dimensions Cache Dimensions
 * The OLED cache is organized as:
 * - **Height**: @ref CACHED_HEIGHT = 8 pixels (1 byte in vertical compression)
 * - **Width**: @ref CACHED_WIDTH = 144 pixels
 * - **Total Pixels**: 1152 pixels
 * - **Memory Layout**: Packed vertically (8 pixels per byte)
 *
 * @section oled_cache_layout Memory Layout
 * The cache uses a page-based layout common to SSD1306-compatible displays:
 * @code
 * Byte layout (vertical compression):
 * Bit 7: Page 0, Row 7
 * Bit 6: Page 0, Row 6
 * Bit 5: Page 0, Row 5
 * Bit 4: Page 0, Row 4
 * Bit 3: Page 0, Row 3
 * Bit 2: Page 0, Row 2
 * Bit 1: Page 0, Row 1
 * Bit 0: Page 0, Row 0
 * @endcode
 *
 * @section oled_cache_operations Typical Cache Operations
 *
 * @subsection oled_cache_pixel Setting a Pixel
 * @code{.c}
 * void set_pixel(int x, int y, int color) {
 *     if (x < 0 || x >= CACHED_WIDTH || y < 0 || y >= CACHED_HEIGHT * 8) {
 *         return;  // Out of bounds
 *     }
 *
 *     int page = y / 8;      // Which byte (page)
 *     int bit = y % 8;       // Which bit within byte
 *     int offset = page * CACHED_WIDTH + x;
 *
 *     if (color) {
 *         cache[offset] |= (1 << bit);   // Set pixel
 *     } else {
 *         cache[offset] &= ~(1 << bit);  // Clear pixel
 *     }
 * }
 * @endcode
 *
 * @subsection oled_cache_update Updating Display from Cache
 * @code{.c}
 * void display_refresh(void) {
 *     for (int page = 0; page < CACHED_HEIGHT; page++) {
 *         // Set page address command
 *         write_command(0xB0 | page);
 *
 *         // Write entire page (144 bytes)
 *         for (int x = 0; x < CACHED_WIDTH; x++) {
 *             int offset = page * CACHED_WIDTH + x;
 *             write_data(cache[offset]);
 *         }
 *     }
 * }
 * @endcode
 *
 * @section oled_cache_performance Performance Considerations
 * - **Memory**: (CACHED_HEIGHT * 8) * CACHED_WIDTH / 8 bytes = 1152 bytes
 * - **Update Time**: Full refresh ~1-2ms at 400kHz I2C
 * - **Partial Updates**: Recommended for animations (update only changed regions)
 */

/**
 * @defgroup CFBD_OLED_Cache OLED Display Cache Configuration
 * @brief OLED display framebuffer dimensions and settings
 * @ingroup cfbd_display
 * @details
 * Configures the off-screen pixel cache used by OLED drivers. These
 * constants define the resolution that display drivers must support
 * and the memory layout for pixel operations.
 * @{
 */

/**
 * @def CACHED_HEIGHT
 * @brief Cache height in pixels.
 * @details
 * Specifies the vertical dimension of the display cache. In the typical
 * SSD1306 page-based layout, this represents the number of 8-pixel-tall
 * pages. With CACHED_HEIGHT = 8, the actual vertical resolution is
 * 64 pixels (8 pages × 8 pixels per page).
 *
 * @note Changing this requires updating all driver code that references
 *       display height or page counts.
 *
 * @par Calculation
 * Actual vertical pixels = CACHED_HEIGHT × 8
 */
#define CACHED_HEIGHT (96)

/**
 * @def CACHED_WIDTH
 * @brief Cache width in pixels.
 * @details
 * Specifies the horizontal dimension of the display cache in pixels.
 * With CACHED_WIDTH = 144, the display can show 144 characters at 6 pixels
 * wide, or 128 characters at standard 5-pixel-wide fonts, depending on
 * text rendering implementation.
 *
 * @note Changing this requires updating all driver code that handles
 *       horizontal pixel operations, boundaries, and wrapping.
 *
 * @par Total Memory
 * Cache size = (CACHED_HEIGHT × 8 × CACHED_WIDTH) / 8 bytes
 *            = (64 × 144) / 8 = 1152 bytes
 *
 * @par Example - Buffer Allocation
 * @code{.c}
 * // Static allocation of OLED cache
 * static uint8_t oled_cache[CACHED_HEIGHT * CACHED_WIDTH];
 *
 * // Total memory: 8 * 144 = 1152 bytes
 * @endcode
 */
#define CACHED_WIDTH (64)

/** @} */
