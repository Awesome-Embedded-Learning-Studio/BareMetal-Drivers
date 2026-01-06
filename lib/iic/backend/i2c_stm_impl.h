#pragma once
#include "../iic.h"
#include "lib_settings.h"

typedef struct
{
    I2C_HandleTypeDef* hi2c;
    GPIO_TypeDef* scl_port;
    uint16_t scl_pin;
    GPIO_TypeDef* sda_port;
    uint16_t sda_pin;
    /* Miss assigned */
    int last_err;
} CFBD_ST_I2CPrivate;

static inline I2C_HandleTypeDef* native_handle(CFBD_ST_I2CPrivate* priv)
{
    return priv->hi2c;
}

void init_stm32_i2c_privates(CFBD_ST_I2CPrivate* priv,
                             I2C_HandleTypeDef* hi2c,
                             GPIO_TypeDef* scl_port,
                             uint16_t scl_pin,
                             GPIO_TypeDef* sda_port,
                             uint16_t sda_pin);

void stm32_i2c_bus_register(CFBD_I2CHandle* bus, CFBD_ST_I2CPrivate* priv);
