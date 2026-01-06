#include "gpio-stm-impl.h"
#include "lib_settings.h"
#include "gpio.h"
#include "stm32f1xx_hal_gpio.h"

#if defined (CFBD_IS_ST)

static inline void GPIO_CLK_Enable(GPIO_TypeDef *GPIOx)
{
    if (GPIOx == GPIOA) {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    } else if (GPIOx == GPIOB) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    } else if (GPIOx == GPIOC) {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    } else if (GPIOx == GPIOD) {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    } else if (GPIOx == GPIOE) {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    } 
#if defined(GPIOF)
    else if (GPIOx == GPIOF) {
        __HAL_RCC_GPIOF_CLK_ENABLE();
    }
#endif
#if defined(GPIOG)
    else if (GPIOx == GPIOG) {
        __HAL_RCC_GPIOG_CLK_ENABLE();
    }
#endif
    else {
        /* 非法 GPIO，忽略或断言 */
    }
}

static inline void GPIO_CLK_Disable(GPIO_TypeDef *GPIOx)
{
    if (GPIOx == GPIOA) {
        __HAL_RCC_GPIOA_CLK_DISABLE();
    } else if (GPIOx == GPIOB) {
        __HAL_RCC_GPIOB_CLK_DISABLE();
    } else if (GPIOx == GPIOC) {
        __HAL_RCC_GPIOC_CLK_DISABLE();
    } else if (GPIOx == GPIOD) {
        __HAL_RCC_GPIOD_CLK_DISABLE();
    } else if (GPIOx == GPIOE) {
        __HAL_RCC_GPIOE_CLK_DISABLE();
    }
#if defined(GPIOF)
    else if (GPIOx == GPIOF) {
        __HAL_RCC_GPIOF_CLK_DISABLE();
    }
#endif
#if defined(GPIOG)
    else if (GPIOx == GPIOG) {
        __HAL_RCC_GPIOG_CLK_DISABLE();
    }
#endif
    else {
        /* 非法 GPIO，忽略或断言 */
    }
}

void CFBD_GPIOInit(CFBD_GPIOHandle* handle, CFBD_GPIOInitParams params){
    CFBD_ST_GPIOInitParams* st_params = (CFBD_ST_GPIOInitParams*)params;
    handle->handle_internals_ = st_params->handle;
    handle->pin_internals_ = st_params->pin;
    GPIO_CLK_Enable(st_params->handle);
    HAL_GPIO_Init(st_params->handle, &st_params->initer);
}

void CFBD_GPIOSet(CFBD_GPIOHandle* handle){
    HAL_GPIO_WritePin(
        handle->handle_internals_, 
        handle->pin_internals_, GPIO_PIN_SET);
}

void CFBD_GPIOUnset(CFBD_GPIOHandle* handle){
    HAL_GPIO_WritePin(
        handle->handle_internals_, 
        handle->pin_internals_, GPIO_PIN_RESET);
}

void CFBD_GPIOToggle(CFBD_GPIOHandle* handle){
    HAL_GPIO_TogglePin(handle->handle_internals_, 
        handle->pin_internals_);
}
#endif