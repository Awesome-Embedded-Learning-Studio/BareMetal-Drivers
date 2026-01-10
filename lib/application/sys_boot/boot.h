/**
 * @file boot.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief Boot / startup helpers and types for application bootstrap sequence.
 *
 * @details
 * This header declares the lightweight types used by the application's
 * bootstrap sequence. It provides a generic argument container type
 * (`CFBD_BootArgs`), a function-pointer type for bootstrap entry points
 * (`CFBD_BootStrapFunc`) and a small tuple type (`CFBDBootTuple`) that
 * associates an entry function with its arguments.
 *
 * The design aims to be minimal and portable across platforms used in
 * this repository. Implementations of bootstrap functions should adhere
 * to the contract documented for `CFBD_BootStrapFunc`.
 *
 * @page boot_page Boot System Design
 * @brief Overview of the Boot System
 *
 * @section boot_intro Introduction
 * The CFBD Boot System provides a lightweight, generic framework for managing
 * application startup sequences. It separates bootstrap logic from main application
 * code, enabling modular initialization of hardware, drivers, and application layers.
 *
 * @section boot_architecture Architecture
 * The boot system consists of three core components:
 * - **Bootstrap Arguments** (@ref CFBD_BootArgs): Opaque containers for configuration data
 * - **Bootstrap Functions** (@ref CFBD_BootStrapFunc): Entry points executed during startup
 * - **Bootstrap Tuples** (@ref CFBDBootTuple): Associations between functions and their arguments
 *
 * @section boot_workflow Typical Workflow
 * 1. Define boot configuration structures for each initialization stage
 * 2. Create bootstrap functions that operate on those configurations
 * 3. Build a static boot table using `CFBDBootTuple` entries
 * 4. Iterate the table and invoke each function sequentially
 * 5. Handle failures by stopping the sequence or triggering recovery
 *
 * @version 0.1
 * @date 2026-01-09
 * @copyright Copyright (c) 2026
 */

#pragma once
#include "cfbd_define.h"

/**
 * @defgroup CFBD_Boot Boot
 * @brief Minimal boot/bootstrap types and helpers
 * @details Types used to describe and invoke bootstrap/startup routines.
 *
 * The CFBD_Boot module provides a generic, extensible mechanism for managing
 * application startup sequences. It decouples initialization logic from the
 * main application, enabling a clean modular architecture where each subsystem
 * (hardware, drivers, services) registers its own bootstrap function.
 *
 * @ingroup cfbd_system
 * @{
 */

/**
 * @typedef CFBD_BootArgs
 * @brief Opaque pointer to bootstrap-specific arguments.
 *
 * @details
 * Bootstrap argument containers are intentionally typedeffed to `void*`
 * to keep the interface generic. Callers should cast to a concrete
 * structure type known to both the caller and the bootstrap function.
 *
 * @par Example - Basic Argument Casting
 * @code{.c}
 * // Define a boot-time configuration structure
 * struct MyBootConfig {
 *     uint32_t clock_frequency;
 *     uint8_t device_id;
 * } my_config = { 72000000, 0x01 };
 *
 * // Cast to generic CFBD_BootArgs
 * CFBD_BootArgs args = (CFBD_BootArgs)&my_config;
 *
 * // In the bootstrap function, cast back to concrete type
 * CFBD_Bool my_bootstrap(CFBD_BootArgs args) {
 *     struct MyBootConfig *cfg = (struct MyBootConfig *)args;
 *     initialize_clock(cfg->clock_frequency);
 *     return CFBD_True;
 * }
 * @endcode
 *
 * @see CFBDBootTuple
 * @see CFBD_BootStrapFunc
 * @since 0.1
 */
typedef void* CFBD_BootArgs;

/**
 * @typedef CFBD_BootStrapFunc
 * @brief Function-pointer type for bootstrap/startup routines.
 *
 * @details
 * Bootstrap functions receive a single opaque argument (`CFBD_BootArgs`)
 * and return a `CFBD_Bool` indicating success (`CFBD_True`) or failure
 * (`CFBD_False`). Implementations should perform any necessary
 * initialization and return quickly. Long-running tasks or blocking
 * operations should be avoided unless explicitly required by the
 * platform.
 *
 * @param args Opaque pointer to bootstrap-specific arguments (may be NULL).
 * @return CFBD_Bool Return `CFBD_True` on success, `CFBD_False` on failure.
 *
 * @par Example - Simple Bootstrap Function
 * @code{.c}
 * // Hardware initialization bootstrap function
 * CFBD_Bool boot_hardware(CFBD_BootArgs args) {
 *     struct HwConfig *hw_cfg = (struct HwConfig *)args;
 *
 *     // Initialize clock
 *     if (!init_system_clock(hw_cfg->clock_freq)) {
 *         return CFBD_False;
 *     }
 *
 *     // Initialize GPIO
 *     if (!init_gpio_subsystem(hw_cfg->gpio_config)) {
 *         return CFBD_False;
 *     }
 *
 *     return CFBD_True;
 * }
 * @endcode
 *
 * @par Example - Bootstrap Function with Null Arguments
 * @code{.c}
 * // Application initialization that doesn't need configuration
 * CFBD_Bool boot_application(CFBD_BootArgs args) {
 *     (void)args;  // Unused
 *
 *     app_init_subsystems();
 *     app_start_scheduler();
 *
 *     return CFBD_True;
 * }
 * @endcode
 *
 * @note The exact concrete type behind `CFBD_BootArgs` is application
 *       specific. Use `@ref CFBD_BootArgs` casts to access fields.
 *
 * @see CFBD_BootArgs
 * @see CFBDBootTuple
 * @since 0.1
 */
typedef CFBD_Bool (*CFBD_BootStrapFunc)(CFBD_BootArgs args);

/**
 * @typedef CFBDBootTuple
 * @brief Associates a bootstrap function with its arguments.
 *
 * @details
 * This small struct is useful when building static tables or lists of
 * startup actions. Each tuple pairs a `CFBD_BootStrapFunc` with the
 * `CFBD_BootArgs` to be passed when invoking the function.
 *
 * @par Example - Building a Boot Table
 * @code{.c}
 * // Define configurations for each boot stage
 * struct HwConfig hw_config = {
 *     .clock_freq = 72000000,
 *     .gpio_config = &gpio_cfg
 * };
 *
 * struct DriverConfig driver_config = {
 *     .uart_baudrate = 115200,
 *     .spi_speed = 1000000
 * };
 *
 * // Create boot table
 * static const CFBDBootTuple boot_sequence[] = {
 *     {
 *         .boot_func = boot_hardware,
 *         .args = (CFBD_BootArgs)&hw_config
 *     },
 *     {
 *         .boot_func = boot_drivers,
 *         .args = (CFBD_BootArgs)&driver_config
 *     },
 *     {
 *         .boot_func = boot_application,
 *         .args = NULL  // Application doesn't need config
 *     }
 * };
 *
 * // Execute boot sequence
 * CFBD_Bool execute_boot(void) {
 *     size_t boot_count = sizeof(boot_sequence) / sizeof(boot_sequence[0]);
 *
 *     for (size_t i = 0; i < boot_count; i++) {
 *         CFBD_Bool result = boot_sequence[i].boot_func(boot_sequence[i].args);
 *         if (!result) {
 *             // Handle boot failure
 *             handle_boot_failure(i);
 *             return CFBD_False;
 *         }
 *     }
 *     return CFBD_True;
 * }
 * @endcode
 *
 * @see CFBD_BootStrapFunc
 * @see CFBD_BootArgs
 * @since 0.1
 */
typedef struct
{
    /**
     * @brief Pointer to the bootstrap function to invoke.
     *
     * Must conform to the `CFBD_BootStrapFunc` contract: accept a
     * `CFBD_BootArgs` and return `CFBD_Bool`.
     */
    CFBD_BootStrapFunc boot_func;

    /**
     * @brief Opaque pointer passed to `boot_func` when invoked.
     *
     * Interpretation of this pointer is implementation-defined and must
     * match what the `boot_func` expects.
     */
    CFBD_BootArgs args;
} CFBDBootTuple;

/** @} */