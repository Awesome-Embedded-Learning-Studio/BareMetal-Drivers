#pragma once
#include "cfbd_define.h"

typedef void* CFBD_BootArgs;
typedef CFBD_Bool (*CFBD_BootStrapFunc)(CFBD_BootArgs);

typedef struct
{
    CFBD_BootStrapFunc boot_func;
    CFBD_BootArgs args;
} CFBDBootTuple;