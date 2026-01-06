#pragma once
#include "../config/lib_settings.h"

#if defined(CFBD_IS_ST)
#include <stdint.h>
#include <stm32f1xx_hal_gpio.h>

typedef struct
{
    GPIO_TypeDef* handle;
    uint32_t pin;
    GPIO_InitTypeDef initer;
} CFBD_ST_GPIOInitParams; ///< ST GPIO Inits

#endif