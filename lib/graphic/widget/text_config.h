/**
 * @file text_config.h
 * @brief Text widget font configuration and selection
 * @details Provides font size enumeration, selection functions, and configuration
 *          for text rendering. Supports multiple font sizes (6x8, 8x16) with
 *          compile-time selection via resource configuration.
 * @ingroup Graphics_Widget Graphics_Text Graphics_TextConfig
 * @{
 */

#pragma once
#include "base/size.h"
#include "resource/config.h"

/**
 * @defgroup Graphics_TextConfig Text Configuration
 * @ingroup Graphics_Text
 * @brief Font size and text rendering configuration
 * @details Compile-time configuration system for text resources,
 *          controlled by resource/config.h macros.
 * @{
 */

/**
 * @typedef Ascii_Font_Size
 * @brief Enumeration of available ASCII font sizes
 * @details Specifies available monospace font sizes for text rendering.
 *          Enabled fonts are determined at compile-time via:
 *          - @c ENABLE_ASCII_6x8_SOURCES (6x8 pixel font)
 *          - @c ENABLE_ASCII_8x16_SOURCES (8x16 pixel font)
 * @note
 *     Current implementation supports:
 *     - 6x8: 6 pixels wide x 8 pixels tall per character
 *     - 8x16: 8 pixels wide x 16 pixels tall per character
 *     To add more fonts:
 *     1. Provide font bitmap data source
 *     2. Implement __select_from_ascii_font_size() for new size
 *     3. Add enable macro in resource/config.h
 * @see __select_from_ascii_font_size()
 * @see __fetch_font_size()
 * @example
 *     // Using 8x16 font if enabled
 *     #if ENABLE_ASCII_8x16_SOURCES
 *         CFBDGraphic_Text text;
 *         CFBDGraphic_InitText(&text, pos, sz, ASCII_8x16);
 *     #endif
 */
typedef enum
{
#if ENABLE_ASCII_6x8_SOURCES
    /** @brief 6x8 pixel monospace font - compact, suitable for dense text */
    ASCII_6x8,
#endif

#if ENABLE_ASCII_8x16_SOURCES
    /** @brief 8x16 pixel monospace font - standard, good readability */
    ASCII_8x16,
#endif

    /** @brief Sentinel value indicating no valid font size available */
    NO_ASCII_SIZE
} Ascii_Font_Size;

/**
 * @typedef Unicode_Font_Size
 * @brief Enumeration of available Unicode font sizes
 * @details Specifies Unicode font sizes for international character support.
 *          Currently supports 16x16 pixel Unicode characters.
 * @note
 *     Unicode support requires:
 *     - Additional memory for character data (Chinese, Japanese, etc.)
 *     - Larger display for readable text
 *     - Future implementation phase
 * @see Ascii_Font_Size
 */
typedef enum
{
    /** @brief 16x16 pixel Unicode font - for CJK and other wide characters */
    Unicode_16x16
} Unicode_Font_Size;

/**
 * @brief Sentinel value indicating unsupported font source
 * @details Returned by __select_from_ascii_font_size() when:
 *          - Font size is not enabled at compile time
 *          - Font data not available in binary
 *          - Invalid font size requested
 * @note Always check for this value before using font data pointer
 * @see __select_from_ascii_font_size()
 * @example
 *     uint8_t* font = __select_from_ascii_font_size(size, ch);
 *     if (font == UNSUPPORTIVE_FONT_SOURCE) {
 *         // Handle missing font - fallback or error
 *         return ERROR_NO_FONT;
 *     }
 */
#define UNSUPPORTIVE_FONT_SOURCE ((void*) 0)

/**
 * @brief Select font bitmap data for a specific character
 * @details Retrieves bitmap data for rendering a single ASCII character
 *          in the specified font size. Font data must be pre-compiled and linked.
 *          Uses compile-time font selection (ENABLE_ASCII_*x*_SOURCES macros).
 * @param[in] s - Font size to use (ASCII_6x8, ASCII_8x16, etc.)
 * @param[in] ch - ASCII character to render (0-127 for standard ASCII)
 * @return Pointer to font bitmap data for character, or UNSUPPORTIVE_FONT_SOURCE
 *         if font size not enabled at compile time
 * @retval UNSUPPORTIVE_FONT_SOURCE - Font disabled or character out of range
 * @note
 *     - Font data ownership: system (embedded in binary)
 *     - Data lifetime: entire program execution
 *     - Each character bitmap is contiguous in memory
 *     - Bitmap format matches font dimensions (6x8 or 8x16 pixels)
 * @warning
 *     - Behavior undefined if s is not a valid Ascii_Font_Size
 *     - Character code outside ASCII range may return invalid data
 * @example
 *     // Render 'A' in 8x16 font
 *     uint8_t* font_data = __select_from_ascii_font_size(ASCII_8x16, 'A');
 *     if (font_data != UNSUPPORTIVE_FONT_SOURCE) {
 *         // font_data points to 8x16 = 128 bytes of bitmap
 *         draw_character(dev, x, y, font_data, 8, 16);
 *     } else {
 *         // ASCII_8x16 not enabled at compile time
 *         fallback_to_6x8_font();
 *     }
 * @see Ascii_Font_Size
 * @see __fetch_font_size()
 * @see ENABLE_ASCII_6x8_SOURCES
 * @see ENABLE_ASCII_8x16_SOURCES
 */
uint8_t* __select_from_ascii_font_size(const Ascii_Font_Size s, const char ch);

/**
 * @brief Get pixel dimensions of font by size
 * @details Returns the width and height of individual character glyphs
 *          in the specified font size.
 * @param[in] s - Font size enumeration value
 * @return CFBDGraphicSize structure with width and height in pixels
 * @retval width - Character glyph width (6 or 8 pixels for ASCII)
 * @retval height - Character glyph height (8 or 16 pixels for ASCII)
 * @note
 *     - Returns dimensions in pixels
 *     - Includes padding if applicable
 *     - All characters in font are same size (monospace)
 * @example
 *     CFBDGraphicSize font_sz = __fetch_font_size(ASCII_8x16);
 *     printf("Font: %u x %u pixels\n", font_sz.width, font_sz.height);
 *     // Output: Font: 8 x 16 pixels
 *
 *     CFBDGraphic_Point pos = {0, 0};
 *     for (char c = 'A'; c <= 'Z'; c++) {
 *         CFBDGraphic_DrawText(dev, text);
 *         pos.x += font_sz.width;
 *     }
 * @see Ascii_Font_Size
 * @see __select_from_ascii_font_size()
 */
CFBDGraphicSize __fetch_font_size(const Ascii_Font_Size s);

/** @} */