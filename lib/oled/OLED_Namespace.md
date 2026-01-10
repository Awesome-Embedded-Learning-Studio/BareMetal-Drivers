/**
 * @namespace CFBD
 * @brief Core Framework Bare Device (CFBD) namespace
 *
 * @details
 * The CFBD namespace encompasses all core framework types and operations
 * for bare-metal embedded device drivers. This includes display interfaces,
 * communication protocols, and device abstraction layers.
 *
 * The namespace is organized into several submodules:
 * - OLED: Generic OLED display driver subsystem
 * - Device abstraction layers for hardware communication
 * - Type definitions and constants for device control
 */

/**
 * @namespace CFBD::OLED
 * @ingroup OLED
 * @brief OLED display driver subsystem
 *
 * @details
 * The CFBD::OLED namespace contains all OLED-related types, operations,
 * and device drivers. It provides a generic, transport-agnostic interface
 * for controlling OLED displays through various communication backends.
 *
 * Key components:
 * - @ref CFBD_OLED: Main device object
 * - @ref CFBD_OLEDOperations: Virtual operation table
 * - @ref CFBD_OLEDDriverType: Transport type enumeration
 * - Device-specific factories (getSSD1306Specific, getSSD1309Specific)
 */
