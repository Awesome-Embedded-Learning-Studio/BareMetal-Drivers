/**
 * @file define.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief CF_BareMatels libraries defines
 * @version 0.1
 * @date 2026-01-01
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once

#include <stdint.h>

/**
 * @brief Library bools
 *
 */
typedef uint8_t CFBD_Bool;
#define CFBD_FALSE (0)
#define CFBD_TRUE (1)

typedef CFBD_Bool (*SelfInitFunc)(void* self_handle, void* args);
typedef void (*PlainFunc)(void* args);
