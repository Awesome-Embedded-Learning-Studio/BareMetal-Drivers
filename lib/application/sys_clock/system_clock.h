/**
 * @file system_clock.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2025-03-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef SYSCLK_H
#define SYSCLK_H
#include <stdint.h>

/**
 * @brief delay the global system for given miliseconds
 *
 * @param delay_ms
 */
void system_delay_ms(uint32_t delay_ms);

/**
 * @brief delay the global system for given museconds
 *
 * @param us
 */
void system_delay_us(uint32_t us);

#endif