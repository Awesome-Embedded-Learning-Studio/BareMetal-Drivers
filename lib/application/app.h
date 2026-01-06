#pragma once

#include <stdint.h>

#include "cfbd_define.h"
#include "sys_boot/boot.h"

typedef uint32_t (*CFBD_ClockFreqProvider)(void);

typedef struct __CFBDApplication
{
    CFBD_BootStrapFunc selfBootFunc;
    CFBD_BootArgs bootargs;
    CFBD_ClockFreqProvider tick_provider;
} CFBDApplication;

extern CFBDBootTuple* CFBD_AppBootMaker(void);
extern CFBD_ClockFreqProvider CFBD_AppClockProvider(void);

CFBDApplication* getApp(CFBD_Bool request_auto_boot);
