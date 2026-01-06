#ifndef CCGraphic_Config_H
#define CCGraphic_Config_H

#ifdef _USE_NO_DEFAULT_SOURCES // DISABLE THE SOURCES
#define ENABLE_ASCII_6x8_SOURCES 0
#define ENABLE_ASCII_8X16_SOURCES 0
#else
/* then enable all first */
#define ENABLE_ASCII_6x8_SOURCES 1
#define ENABLE_ASCII_8x16_SOURCES 1

/* disabled one by one */
#ifdef DISABLE_ASCII_6X8_SOURCES
#undef ENABLE_ASCII_6x8_SOURCES
#define ENABLE_ASCII_6x8_SOURCES 0
#endif

#ifdef DISABLE_ASCII_8X16_SOURCES
#undef ENABLE_ASCII_8X16_SOURCES
#define ENABLE_ASCII_8X16_SOURCES 0
#endif

#endif // for the _USE_NO_DEFAULT_SOURCES

#endif // for the no-repeative
