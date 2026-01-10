/**
 * @file text.h
 * @brief Text widget for rendering ASCII text strings
 * @details Provides comprehensive text rendering with support for multiple
 *          font sizes, multiline text, and configurable layout. Text renders
 *          monospace characters with optional padding for visual separation.
 * @ingroup Graphics_Widget Graphics_Text
 * @{
 */

#pragma once
#include <stdint.h>

#include "base/point.h"
#include "base/rectangle.h"
#include "base/size.h"
#include "cfbd_graphic_define.h"
#include "text_config.h"

/**
 * @defgroup Graphics_Text Text Widget System
 * @ingroup Graphics_Widget
 * @brief Text rendering and typography system
 * @details Comprehensive text widget supporting multiple fonts,
 *          multiline rendering, and flexible layout options.
 * @{
 */

/**
 * @brief Default horizontal padding between characters in pixels
 * @details Space added between consecutive characters for visual separation.
 *          Set to 1 pixel for standard monospace layout.
 * @see CFBDGraphic_TEXT_PADDING_HEIGHT
 */
#define CFBDGraphic_TEXT_PADDING_WIDTH (1)

/**
 * @brief Default vertical padding between lines in pixels
 * @details Space added between text lines for multiline text.
 *          Set to 0 for tight line spacing.
 * @see CFBDGraphic_TEXT_PADDING_WIDTH
 */
#define CFBDGraphic_TEXT_PADDING_HEIGHT (0)

/**
 * @typedef CFBDGraphic_Text
 * @brief Text widget structure for rendering text strings
 * @details Encapsulates all state needed for text rendering including
 *          string content, position, size, font, and layout information.
 * @see CFBDGraphic_InitText()
 * @see CFBDGraphic_DrawText()
 */
typedef struct
{
    /** @brief Text string pointer (borrowed - caller owns memory)
     *  @details Zero-terminated C string, must remain valid during rendering */
    char* sources_borrowed;

    /** @brief Top-left corner position of text area in pixels */
    CFBDGraphic_Point tl_point;

    /** @brief Current position for next character (internal state)
     *  @details Tracks insertion point during multiline rendering */
    CFBDGraphic_Point indexed_point;

    /** @brief Allocated size of text rendering area (width x height) in pixels
     *  @details Constrains text layout and line wrapping */
    CFBDGraphicSize TexthandleSize;

    /** @brief Selected font size for text rendering
     *  @details Determines character dimensions (6x8 or 8x16 pixels) */
    Ascii_Font_Size font_size;

    /** @brief Bounding rectangle of rendered text (calculated during draw) */
    CFBDGraphicRect text_bounding_rect;

    CFBD_Bool no_wrap;
} CFBDGraphic_Text;

/**
 * @typedef AppendMethod
 * @brief Text append/layout method enumeration
 * @details Specifies how to layout text when drawing:
 *          - Continuously on same line with wrapping
 *          - Start new line after each character/word
 *          - Query previous render position
 * @see CFBDGraphic_DrawText()
 */
typedef enum
{
    /** @brief Continue on same line, wrap to next when reaching width limit
     *  @details Standard paragraph layout mode */
    CCGraphic_AsciiTextItem_AppendContinously,

    /** @brief Start new line for next text segment
     *  @details Forces line break after rendering */
    CCGraphic_AsciiTextItem_AppendNextLine,

    /** @brief Return to previous line's ending position without drawing
     *  @details Query-only mode: returns position without rendering */
    CCGraphic_AsciiTextItem_RequestOldPoint
} AppendMethod;

/**
 * @brief Initialize text widget structure
 * @details Sets up text widget with position, bounds, and font configuration.
 *          Text content is not set; use CFBDGraphic_SetText() after initialization.
 * @param[out] item - Text structure to initialize
 * @param[in] tl_point - Top-left corner position in pixels
 * @param[in] textHandleSize - Width and height of text rendering area
 * @param[in] text_size - Font size enumeration (ASCII_6x8, ASCII_8x16, etc.)
 * @return void
 * @note Position and size pointers must remain valid for rendering lifetime
 * @warning Text content is not set by this function; call CFBDGraphic_SetText() before drawing
 * @example
 *     CFBDGraphic_Text text;
 *     CFBDGraphic_Point pos = {10, 10};
 *     CFBDGraphicSize area = {118, 54};  // 118x54 pixel area
 *     CFBDGraphic_InitText(&text, pos, area, ASCII_8x16);
 *     CFBDGraphic_SetText(&text, "Hello World");
 *     CFBDGraphic_DrawText(device, &text, CCGraphic_AsciiTextItem_AppendContinously);
 * @see CFBDGraphic_SetText()
 * @see CFBDGraphic_DrawText()
 * @see Ascii_Font_Size
 */
void CFBDGraphic_InitText(CFBDGraphic_Text* item,
                          CFBDGraphic_Point tl_point,
                          CFBDGraphicSize textHandleSize,
                          Ascii_Font_Size text_size);

/**
 * @brief Set text string content
 * @details Associates a text string with the text widget.
 *          String must remain valid until next draw operation.
 * @param[in,out] item - Text widget to update
 * @param[in] text - Zero-terminated C string (caller owned)
 * @return void
 * @note String pointer is stored; string must remain valid during drawing
 * @warning Behavior undefined if text is NULL
 * @example
 *     CFBDGraphic_SetText(&text, "Press Start");
 *     CFBDGraphic_DrawText(device, &text, CCGraphic_AsciiTextItem_AppendContinously);
 * @see CFBDGraphic_DrawText()
 */
static inline void CFBDGraphic_SetText(CFBDGraphic_Text* item, char* text)
{
    item->sources_borrowed = text;
}

/**
 * @brief Set text widget top-left position
 * @details Updates rendering position and resets insertion point to top-left.
 *          Used when repositioning text on display.
 * @param[in,out] item - Text widget to update
 * @param[in] p - New top-left position pointer
 * @return void
 * @note Updates both tl_point and indexed_point to p
 * @example
 *     CFBDGraphic_Point new_pos = {20, 30};
 *     CFBDGraphic_SetTextTLPointPoint(&text, &new_pos);
 *     CFBDGraphic_DrawText(device, &text, CCGraphic_AsciiTextItem_AppendContinously);
 * @see CCGraphicWidget_AsciiTextItem_relocate()
 */
static inline void CFBDGraphic_SetTextTLPointPoint(CFBDGraphic_Text* item, CFBDGraphic_Point* p)
{
    item->tl_point = *p;
    item->indexed_point = *p;
}

/**
 * @brief Set current text insertion point
 * @details Updates only the indexed_point (next character position),
 *          without affecting top-left corner. Useful for resuming multiline rendering.
 * @param[in,out] item - Text widget to update
 * @param[in] p - New insertion point position
 * @return void
 * @note Affects only indexed_point; top-left position unchanged
 * @example
 *     CFBDGraphic_Point continue_pos = {0, 24};  // Next line
 *     CFBDGraphic_SetTextIndexedPoint(&text, &continue_pos);
 * @see CFBDGraphic_SetTextTLPointPoint()
 */
static inline void CFBDGraphic_SetTextIndexedPoint(CFBDGraphic_Text* item, CFBDGraphic_Point* p)
{
    item->indexed_point = *p;
}

/**
 * @brief Relocate text widget to new position and area
 * @details Updates both position and rendering area size.
 *          Used for dynamic layout or repositioning with size change.
 * @param[in,out] item - Text widget to relocate
 * @param[in] tl_point - New top-left corner position
 * @param[in] textHandleSize - New rendering area dimensions
 * @return void
 * @note Both position and size must remain valid for rendering
 * @example
 *     CFBDGraphic_Point new_pos = {0, 0};
 *     CFBDGraphicSize new_area = {128, 64};
 *     CCGraphicWidget_AsciiTextItem_relocate(&text, &new_pos, new_area);
 *     // Text now renders in full display
 * @see CFBDGraphic_SetTextTLPointPoint()
 */
static inline void CCGraphicWidget_AsciiTextItem_relocate(CFBDGraphic_Text* item,
                                                          CFBDGraphic_Point* tl_point,
                                                          CFBDGraphicSize textHandleSize)
{
    item->tl_point = *tl_point;
    item->TexthandleSize = textHandleSize;
}

/**
 * @brief Render text string to graphics device
 * @details Draws text string using character bitmaps from selected font.
 *          Supports multiline layout with configurable append methods.
 *          Returns position after rendered text for continued rendering.
 * @param[in] device_handle - Graphics device to render on
 * @param[in,out] item - Text widget (position updated after rendering)
 * @param[in] method - Text layout method (continuous, newline, query)
 * @return CFBDGraphic_Point - Position after last rendered character
 * @retval x - Next X position for continued text
 * @retval y - Next Y position (may be on new line)
 * @note
 *     - Respects device drawing mode (immediate/deferred)
 *     - Automatically wraps text when reaching width limit
 *     - Updates text_bounding_rect with rendered area
 *     - Character spacing: CFBDGraphic_TEXT_PADDING_WIDTH pixels
 *     - Line spacing: CFBDGraphic_TEXT_PADDING_HEIGHT pixels
 * @warning
 *     - Text pointer must remain valid during draw
 *     - Device must be initialized and opened
 * @example
 *     // Simple text rendering
 *     CFBDGraphic_Point end_pos = CFBDGraphic_DrawText(
 *         device, &text,
 *         CCGraphic_AsciiTextItem_AppendContinously
 *     );
 *     printf("Text ends at (%u, %u)\n", end_pos.x, end_pos.y);
 *
 *     // Multiline text
 *     CFBDGraphic_SetText(&text, "Line 1");
 *     CFBDGraphic_DrawText(device, &text, CCGraphic_AsciiTextItem_AppendContinously);
 *     CFBDGraphic_SetText(&text, "Line 2");
 *     CFBDGraphic_DrawText(device, &text, CCGraphic_AsciiTextItem_AppendNextLine);
 *
 *     // Query next position without drawing
 *     CFBDGraphic_Point query_pos = CFBDGraphic_DrawText(
 *         device, &text,
 *         CCGraphic_AsciiTextItem_RequestOldPoint
 *     );
 * @see CFBDGraphic_SetText()
 * @see CFBDGraphic_InitText()
 * @see AppendMethod
 */
CFBDGraphic_Point CFBDGraphic_DrawText(CFBD_GraphicDevice* device_handle,
                                       CFBDGraphic_Text* item,
                                       AppendMethod method);

/** @} */

/** @} */
