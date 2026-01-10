/**
 * @file cfbd_graphic_define.h
 * @brief Core graphics type definitions and forward declarations
 * @details Provides fundamental type aliases and forward declarations used
 *          throughout the graphics system, establishing the foundation for
 *          point, size, and device abstractions.
 * @ingroup Graphics_Core Graphics_Defines
 * @{
 */

#pragma once
#include <stdint.h>

/**
 * @defgroup Graphics_Defines Graphics Core Definitions
 * @ingroup Graphics_Core
 * @brief Fundamental type definitions for graphics system
 * @{
 */

/**
 * @typedef PointBaseType
 * @brief Base type for point coordinate values
 * @details Used for X and Y coordinates in graphics operations.
 *          Using uint16_t allows coordinates from 0-65535,
 *          supporting displays up to 65K x 65K resolution.
 * @see CFBDGraphic_Point
 */
typedef uint16_t PointBaseType;

/**
 * @typedef SizeBaseType
 * @brief Base type for size dimension values
 * @details Used for width and height dimensions in graphics operations.
 *          Identical to PointBaseType for consistent coordinate/size handling.
 * @see CFBDGraphicSize
 */
typedef PointBaseType SizeBaseType;

/** @} */

/**
 * @brief Forward declaration of graphics device structure
 * @details CFBD_GraphicDevice is the main graphics abstraction providing
 *          rendering operations. This forward declaration enables circular
 *          dependencies in device-related headers without requiring full
 *          device structure definition.
 * @see CFBD_GraphicDevice in graphic_device.h
 */
typedef struct _CFBD_GraphicDevice CFBD_GraphicDevice;

/** @} */
