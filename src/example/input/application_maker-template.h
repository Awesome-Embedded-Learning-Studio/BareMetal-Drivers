#pragma once
#include <stdint.h>

#include "cfbd_define.h"

typedef struct _MyBootArgs
{
    CFBD_Bool should_led_init;
    uint32_t shell_sleep;
} MyBootArgs;
