/**
 * @file config.h
 * @brief Graphics resource configuration and feature toggles.
 * @ingroup Graphics_Resources
 *
 * @details
 * This configuration header controls which graphics resources and features
 * are compiled and available at runtime. It provides conditional compilation
 * flags for optional fonts and resources.
 *
 * Configuration can be controlled in two ways:
 * 1. Define _USE_NO_DEFAULT_SOURCES to disable all resources
 * 2. Define individual DISABLE_* macros to selectively disable resources
 *
 * @see Animation framework
 * @see Graphics device abstraction
 */

#ifndef CCGraphic_Config_H
#define CCGraphic_Config_H

/**
 * @defgroup Graphics_Resources Graphics Resources
 * @ingroup Graphics
 * @brief Resource management and configuration for graphics subsystem.
 *
 * @details
 * This module manages the configuration of graphics resources including
 * character fonts, textures, and other rendering assets. It provides
 * compile-time feature selection and resource management.
 *
 * @{\n */

/**
 * @def _USE_NO_DEFAULT_SOURCES
 * @brief Disable all default graphics resources.
 *
 * @details
 * When defined, disables compilation of all default graphics resources
 * (fonts, etc.). This is useful for space-constrained embedded systems
 * or custom resource builds.
 *
 * When not defined, resources are enabled by default but can be individually
 * disabled using DISABLE_* macros.
 *
 * @see ENABLE_ASCII_6x8_SOURCES
 * @see ENABLE_ASCII_8x16_SOURCES
 *
 * @example
 * @code
 * // To compile without default fonts:
 * #define _USE_NO_DEFAULT_SOURCES
 * #include "config.h"
 *
 * // To selectively disable fonts:
 * #undef _USE_NO_DEFAULT_SOURCES
 * #define DISABLE_ASCII_6X8_SOURCES
 * #include "config.h"
 * @endcode
 */

#ifdef _USE_NO_DEFAULT_SOURCES

/**
 * @def ENABLE_ASCII_6x8_SOURCES
 * @brief Enable/disable 6x8 pixel ASCII font resource.
 *
 * @details
 * Controls compilation of the 6x8 pixel ASCII font. This compact font
 * is suitable for space-constrained displays or dense text rendering.
 *
 * Value: 0 (disabled when _USE_NO_DEFAULT_SOURCES is defined)
 *
 * @note Automatically set to 0 when _USE_NO_DEFAULT_SOURCES is defined.
 *       Can be re-enabled by undefining DISABLE_ASCII_6X8_SOURCES.
 *
 * @see ENABLE_ASCII_8x16_SOURCES
 */
#define ENABLE_ASCII_6x8_SOURCES 0

/**
 * @def ENABLE_ASCII_8X16_SOURCES
 * @brief Enable/disable 8x16 pixel ASCII font resource.
 *
 * @details
 * Controls compilation of the 8x16 pixel ASCII font. This standard font
 * is suitable for general-purpose text rendering with good readability.
 *
 * Value: 0 (disabled when _USE_NO_DEFAULT_SOURCES is defined)
 *
 * @note Automatically set to 0 when _USE_NO_DEFAULT_SOURCES is defined.
 *       Can be re-enabled by undefining DISABLE_ASCII_8X16_SOURCES.
 *
 * @see ENABLE_ASCII_6x8_SOURCES
 */
#define ENABLE_ASCII_8X16_SOURCES 0

#else

/**
 * @def ENABLE_ASCII_6x8_SOURCES
 * @brief Enable/disable 6x8 pixel ASCII font resource.
 *
 * @details
 * Controls compilation of the 6x8 pixel ASCII font. This compact font
 * is useful for dense text or small screens.
 *
 * Default: 1 (enabled)
 * Can be disabled by defining DISABLE_ASCII_6X8_SOURCES.
 */
#define ENABLE_ASCII_6x8_SOURCES 1

/**
 * @def ENABLE_ASCII_8x16_SOURCES
 * @brief Enable/disable 8x16 pixel ASCII font resource.
 *
 * @details
 * Controls compilation of the 8x16 pixel ASCII font. This standard font
 * provides good readability for most applications.
 *
 * Default: 1 (enabled)
 * Can be disabled by defining DISABLE_ASCII_8X16_SOURCES.
 */
#define ENABLE_ASCII_8x16_SOURCES 1

/* Selective resource disabling */

/**
 * @def DISABLE_ASCII_6X8_SOURCES
 * @brief Define to disable 6x8 pixel ASCII font compilation.
 *
 * @details
 * When defined, prevents compilation of the 6x8 ASCII font resource,
 * saving flash/memory space while maintaining other fonts.
 *
 * @example
 * @code
 * #define DISABLE_ASCII_6X8_SOURCES
 * #include "config.h"  // Only 8x16 font will be compiled
 * @endcode
 */
#ifdef DISABLE_ASCII_6X8_SOURCES
#undef ENABLE_ASCII_6x8_SOURCES
#define ENABLE_ASCII_6x8_SOURCES 0
#endif

/**
 * @def DISABLE_ASCII_8X16_SOURCES
 * @brief Define to disable 8x16 pixel ASCII font compilation.
 *
 * @details
 * When defined, prevents compilation of the 8x16 ASCII font resource,
 * saving flash/memory space while maintaining other fonts.
 *
 * @example
 * @code
 * #define DISABLE_ASCII_8X16_SOURCES
 * #include "config.h"  // Only 6x8 font will be compiled
 * @endcode
 */
#ifdef DISABLE_ASCII_8X16_SOURCES
#undef ENABLE_ASCII_8X16_SOURCES
#define ENABLE_ASCII_8X16_SOURCES 0
#endif

#endif // for the _USE_NO_DEFAULT_SOURCES

#endif // for the no-repeative include guard

/** @} */ // end of Graphics_Resources group
