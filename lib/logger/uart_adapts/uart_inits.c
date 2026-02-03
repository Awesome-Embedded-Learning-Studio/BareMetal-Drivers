#pragma once
#include <stdint.h>

#include "app.h"
#include "cfbd_define.h"
#include "cfbd_log.h"
#include "cfbd_log_configs.h"
#include "uart.h"

static uint8_t logger_buffer[CFBD_LOG_BUFFER_SZ];

static inline CFBD_UARTHandle* asUARTNative(void* native_handle)
{
    return (CFBD_UARTHandle*) native_handle;
}

static void log_native_sync(void* native_handle, const char* buffer, const uint16_t buffer_size)
{
    CFBD_UARTHandle* uart = asUARTNative(native_handle);
    uart->ops->sync_send(uart, (uint8_t*) buffer, buffer_size);
}

static void log_native_async(void* native_handle, const char* buffer, const uint16_t buffer_size)
{
    CFBD_UARTHandle* uart = asUARTNative(native_handle);
    uart->ops->async_send(uart, (uint8_t*) buffer, buffer_size);
}

static uint32_t log_timestamp(void* _)
{
    return getApp(CFBD_FALSE)->tick_provider();
}

static CFBD_NativeLoggerOperations sync_ops = {.log_native = log_native_sync,
                                               .timestamp = log_timestamp};

static CFBD_NativeLoggerOperations async_ops = {.log_native = log_native_async,
                                                .timestamp = log_timestamp};

void CFBD_InitLoggerWithUART(CFBD_Logger* logger,
                             CFBD_UARTHandle* uart_handle,
                             cfbd_log_level_t filter_level,
                             CFBD_Bool async_ok)
{
    if (async_ok) {
        CFBD_InitLogger(logger,
                        &async_ops,
                        uart_handle,
                        filter_level,
                        logger_buffer,
                        CFBD_LOG_BUFFER_SZ);
    }
    else {
        CFBD_InitLogger(logger,
                        &sync_ops,
                        uart_handle,
                        filter_level,
                        logger_buffer,
                        CFBD_LOG_BUFFER_SZ);
    }
}