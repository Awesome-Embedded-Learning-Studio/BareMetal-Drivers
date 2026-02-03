#pragma once

#include "cfbd_log.h"
#include "uart.h"

void CFBD_InitLoggerWithUART(CFBD_Logger* logger,
                             CFBD_UARTHandle* uart_handle,
                             cfbd_log_level_t filter_level,
                             CFBD_Bool async_ok);