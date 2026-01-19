/**
 * @file gpio-stm-impl.h
 * @brief STM32 specific GPIO implementation details for CFBD.
 * @ingroup GPIO_Module
 *
 * @details
 * This header provides STM32-specific types used by the public GPIO
 * abstraction (`lib/gpio/gpio.h`). It is included only when the
 * `CFBD_IS_ST` macro selects the ST platform implementation. The
 * declarations here reference HAL types from `stm32f1xx_hal_gpio.h`.
 *
 * The STM32 GPIO backend encapsulates STM HAL initialization parameters
 * and provides the low-level interface for pin configuration, setting,
 * clearing and toggling operations on STM32 microcontrollers.
 *
 * @section stm_gpio_usage STM32 GPIO Usage
 *
 * To use STM32 GPIO functionality:
 * 1. Include `gpio.h` (which conditionally includes this file when CFBD_IS_ST is set)
 * 2. Populate a `CFBD_ST_GPIOInitParams` structure with HAL types
 * 3. Cast to `CFBD_GPIOInitParams` and pass to `CFBD_GPIOInit`
 *
 * @section stm_gpio_example STM32 GPIO Example
 *
 * @code{.c}
 * #include "lib/gpio/gpio.h"
 * #include <stm32f1xx_hal.h>
 *
 * // Example: Initialize GPIOA pin 5 as output
 * CFBD_ST_GPIOInitParams stm_params;
 * stm_params.handle = GPIOA;
 * stm_params.pin = GPIO_PIN_5;
 * stm_params.initer.Mode = GPIO_MODE_OUTPUT_PP;
 * stm_params.initer.Pull = GPIO_NOPULL;
 * stm_params.initer.Speed = GPIO_SPEED_FREQ_HIGH;
 *
 * CFBD_GPIOHandle gpio_handle;
 * CFBD_GPIOInit(&gpio_handle, (CFBD_GPIOInitParams)&stm_params);
 *
 * // Set pin high
 * CFBD_GPIOSet(&gpio_handle);
 * @endcode
 */

#pragma once
#include "../config/lib_settings.h"

#if defined(CFBD_IS_ST)
#include <stdint.h>

/**
 * @defgroup STM32_GPIO_Impl STM32 GPIO Implementation Backend
 * @ingroup GPIO_Module
 * @brief Platform-specific GPIO implementation for STM32 microcontrollers.
 * @details Provides STM32 HAL integration layer for GPIO operations.
 * @{
 */

/**
 * @typedef CFBD_ST_GPIOInitParams
 * @brief Initialization parameters for STM32 GPIO backend.
 *
 * @details
 * Aggregate that contains the HAL peripheral pointer, the pin value
 * and the requested initial configuration (`GPIO_InitTypeDef`) used by
 * the STM32 GPIO implementation. This structure is intended to be
 * passed (cast to `void*`) through the public `CFBD_GPIOInit` API when
 * running on ST targets.
 *
 * This structure bridges the public portable GPIO API with STM32 HAL-specific
 * types, allowing platform-agnostic code to work seamlessly with STM32 devices.
 *
 * @see CFBD_GPIOInit
 * @see CFBD_GPIOHandle
 */
typedef struct
{
    /**
     * @brief Pointer to the HAL GPIO peripheral instance (e.g. GPIOA, GPIOB).
     * @details Must be a valid GPIO port as defined by the STM32 HAL library.
     */
    GPIO_TypeDef* handle;

    /**
     * @brief Pin mask or number identifying the pin within the port.
     * @details Typically uses GPIO_PIN_0 through GPIO_PIN_15 or corresponding masks.
     */
    uint32_t pin;

    /**
     * @brief HAL initialization structure describing mode, pull and speed.
     * @details Fully configures the pin behavior (output/input, push-pull/open-drain, etc.).
     * @see GPIO_InitTypeDef (from stm32f1xx_hal_gpio.h)
     */
    GPIO_InitTypeDef initer;
} CFBD_ST_GPIOInitParams; ///< ST GPIO initialization parameters

/** @} */

#endif