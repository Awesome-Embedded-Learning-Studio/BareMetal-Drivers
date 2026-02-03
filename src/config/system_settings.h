/**
 * @file system_settings.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief config the system here
 * @version 0.1
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#if defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || defined(STM32F3) ||                \
        defined(STM32F4) || defined(STM32F7) || defined(STM32G0) || defined(STM32G4) ||            \
        defined(STM32H7) || defined(STM32L0) || defined(STM32L1) || defined(STM32L4)

#define CFBD_IS_ST (1)

#if defined(STM32F1)
// includes for chip libraries
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_uart.h>
#endif

#if defined(STM32G4)
#include <stm32g4xx_hal.h>
#include <stm32g4xx_hal_gpio.h>
#include <stm32g4xx_hal_uart.h>
#endif

#else

#define CFBD_IS_ST (0)

#endif
