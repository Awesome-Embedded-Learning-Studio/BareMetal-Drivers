/**
 * @file app.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Application descriptor and clock/boot provider interfaces.
 * @ingroup Application_Module
 *
 * @details
 * This header defines the `CFBDApplication` structure which aggregates
 * application-specific bootstrap and clock provider hooks used by the
 * framework. It also declares helper functions to obtain the global
 * application's boot table and clock/tick providers.
 *
 * Implementations in the application layer should provide matching
 * definitions for the providers declared here. Keep these interfaces
 * small to allow linking a minimal runtime for constrained targets.
 *
 * @version 0.1
 * @date 2026-01-10
 *
 * @page application_setup Application Initialization and Setup
 * @section app_overview Overview
 * The application module provides a unified interface for initializing
 * and managing an embedded application's boot sequence and clock providers.
 * This design allows applications to register their initialization routines
 * and timing sources with the CFBD framework.
 *
 * @section app_architecture Architecture
 * The `CFBDApplication` structure acts as a central registry containing:
 * - Bootstrap function and parameters for system initialization
 * - Clock frequency provider for timing calculations
 * - Tick counter provider for interval measurement
 *
 * @section app_usage Basic Application Setup
 * @code{.c}
 * #include "lib/application/app.h"
 *
 * // Clock providers (from platform code)
 * uint32_t get_system_clock_hz(void) {
 *     return 72000000;  // STM32F103 at 72 MHz
 * }
 *
 * uint32_t get_tick_count(void) {
 *     return SysTick->VAL;  // Read from system timer
 * }
 *
 * // Application bootstrap
 * CFBD_Bool app_bootstrap(void* self_handle, void* args) {
 *     // Initialize peripherals
 *     init_gpio();
 *     init_uart();
 *     return CFBD_TRUE;
 * }
 *
 * // Application instance
 * CFBDApplication g_app = {
 *     .selfBootFunc = app_bootstrap,
 *     .bootargs = NULL,
 *     .freq_provider = get_system_clock_hz,
 *     .tick_provider = get_tick_count
 * };
 *
 * // Application retrieval
 * CFBDApplication* getApp(CFBD_Bool request_auto_boot) {
 *     if (request_auto_boot && g_app.selfBootFunc) {
 *         g_app.selfBootFunc(&g_app, g_app.bootargs);
 *     }
 *     return &g_app;
 * }
 * @endcode
 *
 * @section app_timing Timing and Clock Coordination
 * @code{.c}
 * // Get current timing information
 * CFBDApplication* app = getApp(CFBD_FALSE);
 * uint32_t freq = app->freq_provider();  // Get clock frequency
 * uint32_t ticks = app->tick_provider(); // Get current tick count
 *
 * // Calculate delay in milliseconds
 * uint32_t ms_per_tick = 1000 / freq;
 * @endcode
 */

#pragma once

#include <stdint.h>

#include "cfbd_define.h"
#include "sys_boot/boot.h"

/**
 * @defgroup Application_Module Application Management
 * @brief Application-level bootstrap and clock providers
 * @details
 * Provides the core application descriptor and helper functions for
 * managing application initialization and timing services. This module
 * allows applications to register their bootstrap sequences and clock
 * providers with the CFBD framework.
 * @{
 */

/**
 * @typedef CFBD_ClockFreqProvider
 * @brief Function returning the system clock frequency in Hertz.
 *
 * @details
 * Provider returns the current core/system clock frequency in Hz. This
 * value is used by timing helpers and drivers to convert between ticks
 * and wall-clock time. If the platform supports dynamic clock scaling,
 * the provider should return the up-to-date frequency when called.
 *
 * Implementations must ensure the returned value is accurate and stable
 * for use in timing-critical calculations. Typical implementations read
 * clock configuration registers or return a hardcoded constant.
 *
 * @return uint32_t Current clock frequency in Hertz (Hz).
 *
 * @par Example
 * @code{.c}
 * uint32_t stm32_clock_freq(void) {
 *     // STM32F103 typically runs at 72 MHz
 *     return 72000000;
 * }
 * @endcode
 *
 * @see CFBD_ClockTickProvider
 * @see CFBDApplication
 */
typedef uint32_t (*CFBD_ClockFreqProvider)(void);

/**
 * @typedef CFBD_ClockTickProvider
 * @brief Function returning a monotonically increasing tick value.
 *
 * @details
 * Provider returns a tick counter value (units are implementation
 * defined, typically processor ticks or a hardware timer count). The
 * value should be monotonically increasing and suitable for measuring
 * short intervals when combined with the frequency provider.
 *
 * Common implementations include:
 * - System timer (SysTick) count for ARM Cortex-M processors
 * - Hardware timer register values
 * - Software tick counters incremented by interrupts
 *
 * @return uint32_t Current tick counter value. Should increment steadily
 *         and wrap around gracefully.
 *
 * @par Example
 * @code{.c}
 * extern volatile uint32_t g_systick_count;
 *
 * uint32_t get_systick(void) {
 *     return g_systick_count;
 * }
 * @endcode
 *
 * @see CFBD_ClockFreqProvider
 * @see CFBDApplication
 */
typedef uint32_t (*CFBD_ClockTickProvider)(void);

/**
 * @brief Application descriptor.
 *
 * @details
 * Aggregates the bootstrap function and clock providers for an
 * application. Typical usage is to expose a single application
 * instance via `getApp()` that other framework components query to
 * obtain boot actions and timing sources.
 *
 * The application descriptor serves as a central registry for all
 * application-level services, allowing framework components to access
 * initialization routines and timing information without requiring
 * direct coupling to application code.
 *
 * @see getApp()
 * @see CFBD_AppBootMaker()
 * @see CFBD_AppClockProvider()
 * @see CFBD_AppTickProvider()
 */
typedef struct __CFBDApplication
{
    /**
     * @brief Optional bootstrap function for the application.
     *
     * When non-NULL, this function will be called during application
     * startup to perform any app-specific initialization. Must match
     * the `CFBD_BootStrapFunc` signature.
     *
     * @note If NULL, the application framework will skip this step
     *       and rely on explicit boot table instead.
     */
    CFBD_BootStrapFunc selfBootFunc;

    /**
     * @brief Opaque pointer passed to `selfBootFunc` when invoked.
     *
     * @details Interpretation is defined by the bootstrap function.
     * Common usage includes passing a configuration structure or
     * application-specific context.
     */
    CFBD_BootArgs bootargs;

    /**
     * @brief Provider that returns the system clock frequency (Hz).
     *
     * @details If NULL, timing calculations will fail. All timing-sensitive
     * operations require this provider to be set.
     *
     * @see CFBD_ClockFreqProvider
     */
    CFBD_ClockFreqProvider freq_provider;

    /**
     * @brief Provider that returns a monotonically increasing tick value.
     *
     * @details If NULL, interval measurements will be unavailable.
     * Used in conjunction with freq_provider for time calculations.
     *
     * @see CFBD_ClockTickProvider
     */
    CFBD_ClockTickProvider tick_provider;
} CFBDApplication;

/**
 * @brief Return a pointer to a boot tuple table for the application.
 *
 * @details
 * The returned pointer typically refers to a static array of
 * `CFBDBootTuple` entries describing ordered bootstrap actions. The
 * caller must not attempt to free the returned pointer. Return value
 * may be NULL if the application exposes no explicit boot table.
 *
 * The boot table provides a declarative way to specify initialization
 * sequences without requiring explicit function calls.
 *
 * @return CFBDBootTuple* Pointer to the boot tuple table or NULL.
 *
 * @see CFBDBootTuple
 * @see CFBD_BootStrapFunc
 */
extern CFBDBootTuple* CFBD_AppBootMaker(void);

/**
 * @brief Return the application's clock frequency provider.
 *
 * @details
 * Retrieves the frequency provider function registered with the
 * application descriptor. This allows framework components to query
 * the system clock frequency without direct application coupling.
 *
 * @return CFBD_ClockFreqProvider Function pointer returning system
 *         frequency in Hz, or NULL if not provided.
 *
 * @see CFBD_ClockFreqProvider
 * @see getApp()
 */
extern CFBD_ClockFreqProvider CFBD_AppClockProvider(void);

/**
 * @brief Return the application's tick provider.
 *
 * @details
 * Retrieves the tick provider function registered with the
 * application descriptor. This allows framework components to measure
 * time intervals without direct application coupling.
 *
 * @return CFBD_ClockTickProvider Function pointer returning current
 *         tick count, or NULL if not provided.
 *
 * @see CFBD_ClockTickProvider
 * @see getApp()
 */
extern CFBD_ClockTickProvider CFBD_AppTickProvider(void);

/**
 * @brief Obtain the global `CFBDApplication` instance.
 *
 * @details
 * Returns a pointer to the global application descriptor. If
 * `request_auto_boot` is true the function may trigger automatic
 * application bootstrap actions according to the platform policy.
 *
 * This is the primary entry point for framework components to access
 * application-level services.
 *
 * @param request_auto_boot If `CFBD_TRUE`, request that the application
 *                          performs any auto-boot actions before
 *                          returning (implementation-defined).
 *                          If `CFBD_FALSE`, returns the descriptor
 *                          without triggering bootstrap.
 *
 * @return CFBDApplication* Pointer to the application descriptor, or
 *                         NULL if no application is registered.
 *
 * @par Example: Get application with automatic bootstrap
 * @code{.c}
 * CFBDApplication* app = getApp(CFBD_TRUE);
 * if (app && app->freq_provider) {
 *     uint32_t freq = app->freq_provider();
 * }
 * @endcode
 *
 * @par Example: Get application descriptor without bootstrap
 * @code{.c}
 * CFBDApplication* app = getApp(CFBD_FALSE);
 * // Manually call bootstrap if needed
 * if (app && app->selfBootFunc) {
 *     app->selfBootFunc(app, app->bootargs);
 * }
 * @endcode
 *
 * @see CFBDApplication
 * @see CFBD_BootStrapFunc
 */
CFBDApplication* getApp(CFBD_Bool request_auto_boot);

/** @} */
