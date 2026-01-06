#pragma once
#include "base/size.h"
#include "resource/config.h"

/*
    current version we only support
    6x8 and 8x16. to register more, u should
    provide the source and implement the functions
*/
typedef enum
{
#if ENABLE_ASCII_6x8_SOURCES
    ASCII_6x8,
#endif

#if ENABLE_ASCII_8x16_SOURCES
    ASCII_8x16,
#endif
    NO_ASCII_SIZE
} Ascii_Font_Size;

typedef enum
{
    Unicode_16x16
} Unicode_Font_Size;

#define UNSUPPORTIVE_FONT_SOURCE ((void*) 0)

/**
 * @brief Selects the font data array based on the specified font size.
 *
 * This function receives an `Ascii_Font_Size` value
 * and returns a pointer to the corresponding font data array.
 * The function helps in selecting
 * the appropriate font data for display purposes, allowing for different
 * font sizes (e.g., 8x16, 6x8, etc.).
 *
 * @param s The font size to be selected
 *          (from the `Ascii_Font_Size` enum).
 * @param ch the character wanna display
 * @return  A pointer to the font data array corresponding to the selected font size.
 *          If an invalid font size is passed,
 *          the function returns UNSUPPORTIVE_FONT_SOURCE.
 */
uint8_t* __select_from_ascii_font_size(const Ascii_Font_Size s, const char ch);

CFBDGraphicSize __fetch_font_size(const Ascii_Font_Size s);