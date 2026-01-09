/**
 * @file boot.h
 * @author Charliechen114514 (chengh1922@mails.jlu.edu.cn)
 * @brief
 * @version 0.1
 * @date 2026-01-09
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include "cfbd_define.h"

typedef void* CFBD_BootArgs;
typedef CFBD_Bool (*CFBD_BootStrapFunc)(CFBD_BootArgs);

typedef struct
{
    CFBD_BootStrapFunc boot_func;
    CFBD_BootArgs args;
} CFBDBootTuple;