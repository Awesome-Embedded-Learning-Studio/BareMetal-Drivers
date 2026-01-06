#include "text_config.h"


extern const uint8_t ascii8x16_sources[][16];
extern const uint8_t ascii6x8_sources[][6];

uint8_t* __select_from_ascii_font_size(const Ascii_Font_Size s, const char ch)
{
    switch (s) {
#if ENABLE_ASCII_6x8_SOURCES
        case ASCII_6x8:
            return (uint8_t*) (ascii6x8_sources[ch - ' ']);
#endif
#if ENABLE_ASCII_8x16_SOURCES
        case ASCII_8x16:
            return (uint8_t*) (ascii8x16_sources[ch - ' ']);
#endif
        /*
            To programmers, if new ascii like sources is
            registered, please implement follows
        */
        default:
            return UNSUPPORTIVE_FONT_SOURCE;
    }
}

CFBDGraphicSize __fetch_font_size(const Ascii_Font_Size s)
{
    CFBDGraphicSize size = {0, 0};
    switch (s) {
#if ENABLE_ASCII_6x8_SOURCES
        case ASCII_6x8:
            size.height = 8;
            size.width = 6;
            break;
#endif

#if ENABLE_ASCII_8x16_SOURCES
        case ASCII_8x16:
            size.height = 16;
            size.width = 8;
            break;
#endif
        default:
            break;
    }
    return size;
}
