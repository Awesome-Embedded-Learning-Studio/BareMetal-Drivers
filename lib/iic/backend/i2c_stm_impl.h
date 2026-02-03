/**
 * @file i2c_stm_impl.h
 * @brief STM32 HAL-based I2C backend private types and helpers.
 *
 * @details
 * Provides the STM32-specific private handle used by the project's
 * CFBD I2C abstraction. The private handle contains HAL peripheral
 * references and GPIO pin information used for optional bus recovery
 * or low-level operations. Utility functions are provided to
 * initialize the private structure and register it with the public
 * `CFBD_I2CHandle`.
 *
 * @note This header depends on STM32 HAL types such as
 *       `I2C_HandleTypeDef` and `GPIO_TypeDef` and is only relevant
 *       when building for ST platforms.
 */

#pragma once
#include "../iic.h"

/**
 * @struct CFBD_ST_I2CPrivate
 * @brief Backend-private state for the STM32 I2C implementation.
 *
 * @details
 * Stores references to the HAL I2C peripheral instance and the GPIO
 * pins used for SCL and SDA. The `last_err` field is available to
 * record the most recent backend error and may be consulted by the
 * public `get_error` operation in `CFBD_I2COperations`.
 */
typedef struct
{
    /**
     * @brief Pointer to the HAL `I2C_HandleTypeDef` instance.
     */
    I2C_HandleTypeDef* hi2c;

    /**
     * @brief GPIO port for the SCL line.
     */
    GPIO_TypeDef* scl_port;

    /**
     * @brief GPIO pin number (mask) for SCL.
     */
    uint16_t scl_pin;

    /**
     * @brief GPIO port for the SDA line.
     */
    GPIO_TypeDef* sda_port;

    /**
     * @brief GPIO pin number (mask) for SDA.
     */
    uint16_t sda_pin;

    /**
     * @brief Last backend error code (implementation-defined).
     */
    int last_err;
} CFBD_ST_I2CPrivate;

/**
 * @brief Helper to obtain the native HAL I2C handle from the private struct.
 *
 * @param priv Pointer to `CFBD_ST_I2CPrivate`.
 * @return I2C_HandleTypeDef* Native HAL handle or NULL if `priv` is NULL.
 */
static inline I2C_HandleTypeDef* native_handle(CFBD_ST_I2CPrivate* priv)
{
    return priv ? priv->hi2c : NULL;
}

/**
 * @brief Initialize an STM32 I2C private structure.
 *
 * @details
 * Populates the `priv` structure with supplied HAL handles and GPIO
 * pins. This helper does not modify hardware state; it only prepares
 * the private context prior to registering it with the public bus
 * handle via `stm32_i2c_bus_register()`.
 *
 * @param priv Pointer to the `CFBD_ST_I2CPrivate` instance to initialize.
 * @param hi2c Pointer to a valid `I2C_HandleTypeDef` (HAL instance).
 * @param scl_port GPIO port used for SCL (e.g. GPIOA).
 * @param scl_pin  Pin mask/number for SCL.
 * @param sda_port GPIO port used for SDA (e.g. GPIOA).
 * @param sda_pin  Pin mask/number for SDA.
 */
void init_stm32_i2c_privates(CFBD_ST_I2CPrivate* priv,
                             I2C_HandleTypeDef* hi2c,
                             GPIO_TypeDef* scl_port,
                             uint16_t scl_pin,
                             GPIO_TypeDef* sda_port,
                             uint16_t sda_pin);

/**
 * @brief Register the STM32 private context with a `CFBD_I2CHandle`.
 *
 * @details
 * Associates the backend-private data with the public `CFBD_I2CHandle`.
 * After registration the I2C handle's `ops` table should be populated
 * by the STM32 backend implementation to provide operational
 * functions (init, transfer, etc.).
 *
 * @param bus Pointer to the public `CFBD_I2CHandle` to register.
 * @param priv Pointer to the initialized `CFBD_ST_I2CPrivate`.
 */
void stm32_i2c_bus_register(CFBD_I2CHandle* bus, CFBD_ST_I2CPrivate* priv);
