#pragma once

/**
 * @file ah_no.h
 * @brief Emergency halt helper used when unrecoverable errors occur.
 * @ingroup Core_Utilities
 *
 * @details
 * Provides a tiny, platform-agnostic panic/halt routine intended for
 * fatal error paths where recovery is not possible. The function is
 * implemented as an infinite loop to halt program execution. On many
 * embedded targets a debugger or watchdog will be used to recover from
 * this state; platform-specific implementations may extend this
 * function to signal error LEDs, log information, or trigger a reset.
 *
 * Use with caution — this function does not return.
 *
 * @page error_handling Error Handling Guide
 * @section ah_no_overview Overview
 * The `ah_no.h` header provides emergency halt functionality for handling
 * unrecoverable system errors on bare-metal embedded systems. When a fatal
 * error is detected and continuing execution would be unsafe, applications
 * should invoke `CFBD_AH_NO()` to enter a halt state.
 *
 * @section ah_no_usage Usage Pattern
 * @code{.c}
 * #include "lib/config/ah_no.h"
 *
 * // Example: Halt on critical failure
 * if (critical_resource_failed) {
 *     // Log or signal error if needed
 *     CFBD_AH_NO();  // Enter infinite halt
 *     // Execution never reaches here
 * }
 * @endcode
 *
 * @section ah_no_platform Platform Integration
 * On many platforms, external mechanisms like watchdogs or debuggers
 * can detect and handle the halt state:
 * - Watchdog timers can trigger system reset
 * - Debuggers can break on the infinite loop
 * - Custom firmware may signal error indicators before the halt
 */

/**
 * @defgroup Core_Utilities Core System Utilities
 * @brief System-level utilities for error handling and emergency states.
 * @details These utilities provide critical functionality for managing
 * system state during error conditions and emergency scenarios.
 * @{
 */

/**
 * @brief Enter an infinite halt state for unrecoverable errors.
 *
 * @details
 * This function never returns. It is intended to be called from fatal
 * error paths where continuing execution would be unsafe. Callers may
 * optionally perform architecture-specific diagnostic actions (for
 * example writing to a debug console) before invoking this helper.
 *
 * Common use cases:
 * - Null pointer dereference detection
 * - Memory allocation failures in critical subsystems
 * - Hardware initialization failures
 * - Watchdog timer expiration handling
 * - Stack corruption detection
 *
 * @note Because the function does not return, callers should ensure
 * any required side-effects (flush logs, disable interrupts, etc.)
 * are performed prior to calling.
 *
 * @warning This is a blocking operation that yields control indefinitely.
 * It should only be called when recovery is impossible.
 *
 * @see CFBD_Bool
 * @see CFBD_FALSE
 * @see CFBD_TRUE
 *
 * @par Example
 * @code{.c}
 * #include "lib/config/ah_no.h"
 *
 * void critical_init() {
 *     if (!hardware_initialized) {
 *         // Could log error here before halt
 *         CFBD_AH_NO();  // Never returns
 *     }
 * }
 * @endcode
 */
static inline void CFBD_AH_NO(void)
{
    while (1)
        ;
}

/** @} */
