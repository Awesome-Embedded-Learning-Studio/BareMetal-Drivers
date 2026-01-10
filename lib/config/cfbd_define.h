/**
 * @file cfbd_define.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Core type and macro definitions used across CFBD libraries.
 *
 * @details
 * This header centralizes small, portable typedefs and helper macros
 * used by the CFBD (CF BareMetal Drivers) project. It intentionally
 * contains only minimal, dependency-free declarations so it can be
 * included widely in both platform and application code.
 *
 * The types defined here are lightweight building blocks such as a
 * boolean type and common function-pointer signatures used for
 * component initialization or simple callbacks.
 *
 * @version 0.1
 * @date 2026-01-01
 *
 * @page core_types_guide Core Types and Definitions Guide
 * @section core_intro Introduction
 * The CFBD core type system provides minimal but essential building blocks
 * for embedded systems development. These types ensure consistent behavior
 * across different platforms and compilers.
 *
 * @section core_bool Boolean Type
 * @code{.c}
 * #include "lib/config/cfbd_define.h"
 *
 * // Simple boolean usage
 * CFBD_Bool is_initialized = CFBD_FALSE;
 * if (initialize_system()) {
 *     is_initialized = CFBD_TRUE;
 * }
 * @endcode
 *
 * @section core_callbacks Callback Function Types
 * @code{.c}
 * #include "lib/config/cfbd_define.h"
 *
 * // Initialization callback
 * CFBD_Bool my_init(void* self_handle, void* args) {
 *     // Initialize component
 *     return CFBD_TRUE;  // Success
 * }
 *
 * // Simple callback
 * void my_callback(void* args) {
 *     // Handle event
 * }
 *
 * // Use in your component
 * SelfInitFunc init = my_init;
 * PlainFunc callback = my_callback;
 * @endcode
 */

#pragma once

#include <stdint.h>

/**
 * @defgroup CFBD_Core Core Definitions
 * @brief Small portable types and macros used throughout the project.
 * @details
 * Provides fundamental building blocks for the CFBD framework including
 * platform-independent boolean type, callback function signatures, and
 * related constants. These definitions are designed to be included
 * widely across platform and application code without introducing
 * unnecessary dependencies.
 * @{
 */

/**
 * @typedef CFBD_Bool
 * @brief Minimal boolean type (1 byte).
 *
 * @details
 * Uses an unsigned 8-bit integer to represent boolean values. The
 * explicit width helps keep ABI and header dependencies predictable on
 * constrained targets. This type is portable across different platforms
 * and compilers, making it suitable for use in headers that must be
 * included in diverse contexts.
 *
 * @see CFBD_TRUE
 * @see CFBD_FALSE
 */
typedef uint8_t CFBD_Bool;

/**
 * @def CFBD_FALSE
 * @brief Integer value representing boolean false.
 * @details This macro expands to 0, the standard false value.
 * @see CFBD_Bool
 */
#define CFBD_FALSE (0)

/**
 * @def CFBD_TRUE
 * @brief Integer value representing boolean true.
 * @details This macro expands to 1, the standard true value.
 * @see CFBD_Bool
 */
#define CFBD_TRUE (1)

/**
 * @typedef SelfInitFunc
 * @brief Generic initialization callback signature for objects/components.
 *
 * @details
 * Initialization callbacks receive an opaque `self_handle` which is
 * typically a pointer to the object being initialized, and an opaque
 * `args` pointer for passing platform or application-specific data.
 * Implementations should return `CFBD_TRUE` on successful
 * initialization or `CFBD_FALSE` otherwise.
 *
 * This function pointer type enables a plugin-like architecture where
 * components can be initialized through a common interface without
 * requiring type knowledge at call sites.
 *
 * @param self_handle Pointer to the instance being initialized (may be NULL).
 *                    Typically cast to the specific component type.
 * @param args        Opaque pointer to initialization parameters (may be NULL).
 *                    Interpretation depends on the component implementation.
 * @return CFBD_Bool  `CFBD_TRUE` on successful initialization,
 *                    `CFBD_FALSE` if initialization failed.
 *
 * @see CFBD_Bool
 * @see PlainFunc
 *
 * @par Example
 * @code{.c}
 * CFBD_Bool initialize_component(void* self_handle, void* args) {
 *     // Cast to actual type
 *     ComponentState* component = (ComponentState*)self_handle;
 *     ComponentInitArgs* init_args = (ComponentInitArgs*)args;
 *
 *     if (!component || !init_args) {
 *         return CFBD_FALSE;
 *     }
 *
 *     // Perform initialization
 *     component->initialized = CFBD_TRUE;
 *     return CFBD_TRUE;
 * }
 * @endcode
 */
typedef CFBD_Bool (*SelfInitFunc)(void* self_handle, void* args);

/**
 * @typedef PlainFunc
 * @brief Generic callback taking a single opaque argument and returning void.
 *
 * @details
 * Useful for simple callbacks, event handlers or deferred work items
 * where no return status is required. This function pointer type enables
 * flexible callback registration and invocation without type coupling.
 *
 * @param args Opaque pointer passed to the callback (may be NULL).
 *             Interpretation and lifetime ownership is defined by
 *             the code that registers and invokes the callback.
 *
 * @see SelfInitFunc
 *
 * @par Example
 * @code{.c}
 * void on_timer_tick(void* args) {
 *     TimerContext* ctx = (TimerContext*)args;
 *     ctx->tick_count++;
 * }
 *
 * // Register callback
 * PlainFunc tick_handler = on_timer_tick;
 * tick_handler(timer_context);
 * @endcode
 */
typedef void (*PlainFunc)(void* args);

/** @} */
