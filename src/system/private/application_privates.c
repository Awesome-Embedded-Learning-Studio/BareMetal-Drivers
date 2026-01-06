#include "config/system_settings.h"

/**
 * @brief Systick Handler for the global
 * 
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}