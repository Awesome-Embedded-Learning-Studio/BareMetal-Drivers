#pragma once

#include <stdint.h>

#include "../uart.h"
#include "cfbd_define.h"

typedef struct
{
    UART_HandleTypeDef hal_uart;
    CFBD_UARTHandle* parent_handles;
} CFBD_ST_UARTPrivate;

void CFBD_Init_STUARTBackend(CFBD_UARTHandle* handle,
                             uint32_t timeouts,
                             CFBD_ST_UARTPrivate* st_packs,
                             void (*inits_callback)(CFBD_ST_UARTPrivate* init_callbacks));
