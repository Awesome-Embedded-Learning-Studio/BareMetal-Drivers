/**
 * @file system_clock.c
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2025-03-30
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "system_clock.h"
#include "config/system_settings.h"
#include "app.h"
#include "cfbd_define.h"


/**
 * @brief delay the global system for given museconds
 * 
 * @param us 
 */
void system_delay_us(uint32_t us)
{
    CFBDApplication* app = getApp(CFBD_FALSE);

    uint64_t ticks;                                     // Total number of ticks required for the delay
    uint32_t prevValue, currentValue, elapsedTicks = 0; // Variables for SysTick counter values and elapsed ticks
    uint32_t reloadValue = SysTick->LOAD;               // Value of the SysTick reload register
    uint32_t sysclk_freq = app->tick_provider() / 1000000L;
    ticks = us * sysclk_freq; // Calculate the number of ticks needed for the desired delay in microseconds

    prevValue = SysTick->VAL; // Store the initial SysTick counter value

    while (1)
    {
        currentValue = SysTick->VAL; // Get the current SysTick counter value
        if (currentValue != prevValue)
        {
            if (currentValue < prevValue)
            {
                elapsedTicks += prevValue - currentValue; // Count the elapsed ticks (SysTick is a decrementing counter)
            }
            else
            {
                elapsedTicks += reloadValue - currentValue + prevValue; // Handle the rollover case
            }
            prevValue = currentValue; // Update the previous value to the current value

            if (elapsedTicks >= ticks) // If the elapsed time is greater than or equal to the requested delay
            {
                break; // Exit the loop to complete the delay
            }
        }
    }
}

/**
 * @brief   delay the global system for given miliseconds
 * @note    we must use for loop for delay, as the large decrement makes the
 *          slow deviation for the delay
 * @param delay_ms 
 */
void system_delay_ms(uint32_t delay_ms)
{
    for (uint32_t i = 0; i < delay_ms; i++)
    {
        system_delay_us(1000);
    }
}
